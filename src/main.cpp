#include <mpi.h>
#include <cstdio>

#include "compute.hpp"
#include "routine.hpp"
#include "utils.hpp"

////////////////////////////////////////////////////////////////////////////////

static double target_function(double x)
{
  return 4 / (1 + x * x);
}

static constexpr double FROM = 0.0;
static constexpr double TO   = 1.0;

////////////////////////////////////////////////////////////////////////////////

void PrintUsage(const char *prog_name)
{
  printf("usage: %s n-steps threads-per-process\n\n", prog_name);
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
    PrintUsage(argv[0]);
    return 1;
  }

  compute::Partition prt(compute::Sector{FROM, TO}, strtol(argv[1], NULL, 10));
  size_t n_threads = strtol(argv[2], NULL, 10);

  compute::Routine routine(&argc, &argv);
  if (routine.ErrorCode() != MPI_SUCCESS) {
    routine.Report();
    return 1;
  }

  routine.Run(target_function, prt, n_threads);
  if (routine.ErrorCode() != MPI_SUCCESS) {
    routine.Report();
    return 1;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
