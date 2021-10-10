#include <mpi.h>

#include "routine.hpp"
#include "utils.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace compute {

////////////////////////////////////////////////////////////////////////////////

#ifndef VERBOSE
#define VERBOSITY_LEVEL Logger::LESS
#else
#define VERBOSITY_LEVEL Logger::ALL
#endif

Routine::Routine(int *argc, char ***argv)
  : errno_(MPI_SUCCESS), logger_(VERBOSITY_LEVEL)
{
  errno_ = MPI_Init(argc, argv);
  if (errno_ != MPI_SUCCESS) {
    return;
  }
  GetContext();
}

/* -------------------------------------------------------------------------- */

Routine::~Routine()
{
  errno_ = MPI_Finalize();
  if (errno_ != MPI_SUCCESS) {
    Report();
  }
}

/* -------------------------------------------------------------------------- */

void Routine::GetContext()
{
  errno_ = MPI_Comm_rank(MPI_COMM_WORLD, &rank_);
  if (errno_ != MPI_SUCCESS) {
    return;
  }
  errno_ = MPI_Comm_size(MPI_COMM_WORLD, &world_size_);
}

/* -------------------------------------------------------------------------- */

void Routine::Report() const {
  char err_buf[4096] = "";
  MPI_Error_string(errno_, err_buf, NULL);
  fprintf(stderr, "MPI: %s\n", err_buf);
}

/* -------------------------------------------------------------------------- */

double Routine::RunLocal(compute::RealFunc func, const compute::Partition& prt)
{
  if (rank_ != 0) {
    return 0.0;
  } else {
    logger_.Log(Logger::ALL) << "=== computing integral local ===" << std::endl;
  }

  double local_integral = 0.0;
  double local_time = utils::MeasureMpiTime(
      [&func, &prt, &local_integral]() {
    local_integral = compute::ComputeIntegral(func, prt, 1);
  });

  logger_.Log(Logger::ALL) <<
    "local_integral = " << local_integral << std::endl <<
    "time = " << local_time << "sec" << std::endl;

  return local_time;
}

/* -------------------------------------------------------------------------- */

compute::Partition Routine::ScatterPartition(const compute::Partition& prt)
{
  std::vector<compute::Partition> sub_prt;

  if (rank_ == 0) {
    sub_prt = prt.Split(world_size_);
  }

  compute::Partition local_prt;
  errno_ = MPI_Scatter(sub_prt.data(), sizeof(*sub_prt.data()), MPI_CHAR,
                       &local_prt, sizeof(local_prt), MPI_CHAR,
                       0, MPI_COMM_WORLD);

  return local_prt;
}

/* -------------------------------------------------------------------------- */

std::vector<double> Routine::GatherIntegrals(double local_integral)
{
  std::vector<double> integrals;

  if (rank_ == 0) {
    integrals.resize(world_size_);
  }
  errno_ = MPI_Gather(&local_integral, 1, MPI_DOUBLE,
                      integrals.data(), 1, MPI_DOUBLE,
                      0, MPI_COMM_WORLD);

  return integrals;
}

/* -------------------------------------------------------------------------- */

double Routine::RunDistrib(compute::RealFunc func, const compute::Partition& prt,
                           size_t n_threads)
{
  if (rank_ == 0) {
    logger_.Log(Logger::ALL) <<
      "=== computing integral distributed ===" << std::endl;
  }

  double distrib_integral = 0.0;
  std::vector<double> distrib_integrals;
  double distrib_time = utils::MeasureMpiTime([&]() {
    compute::Partition local_prt = ScatterPartition(prt);
    if (errno_ != MPI_SUCCESS) {
      return;
    }

    double local_integral = compute::ComputeIntegral(func, local_prt, n_threads);

    distrib_integrals = GatherIntegrals(local_integral);
    if (errno_ != MPI_SUCCESS) {
      return;
    }

    if (rank_ == 0) {
      for (auto i : distrib_integrals) {
        distrib_integral += i;
      }
    }
  });

  if (rank_ == 0) {
    for (int i = 0; i < world_size_; ++i) {
      logger_.Log(Logger::ALL) <<
        "proc " << i << " : integral = " << distrib_integrals[i] << std::endl;
    }
    logger_.Log(Logger::ALL) <<
      "distrib_integral = " << distrib_integral << std::endl <<
      "time = " << distrib_time << "sec" << std::endl;
  }

  return distrib_time;
}

/* -------------------------------------------------------------------------- */

void Routine::Run(compute::RealFunc target_func, const compute::Partition& prt,
                  size_t n_threads)
{
  double local_time = RunLocal(target_func, prt);
  MPI_Barrier(MPI_COMM_WORLD);

  double distrib_time = RunDistrib(target_func, prt, n_threads);

  if (rank_ == 0) {
    double acceleration = local_time / distrib_time;
    logger_.Log(Logger::ALL) << "acceleration = " << acceleration << std::endl;
    logger_.Log(Logger::LESS) <<
      prt.StepsCount() << "," << world_size_ << "," << acceleration << std::endl;
  }
}

////////////////////////////////////////////////////////////////////////////////

}  // namespace routine

////////////////////////////////////////////////////////////////////////////////
