#ifndef _ROUTINE_HPP
#define _ROUTINE_HPP

#include "log.hpp"
#include "compute.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace compute {

////////////////////////////////////////////////////////////////////////////////

class Routine {
 public:
  Routine(int *argc, char ***argv);

  ~Routine();

  Routine(const Routine& that) = delete;
  Routine& operator=(const Routine& that) = delete;

  void Report() const;

  int ErrorCode() const {
    return errno_;
  }

  void Run(compute::RealFunc func, const compute::Partition& prt,
           size_t n_threads);

 private:
  void GetContext();

  double RunLocal(compute::RealFunc func, const compute::Partition& prt);

  double RunDistrib(compute::RealFunc func, const compute::Partition& prt,
                    size_t n_threads);

  compute::Partition ScatterPartition(const compute::Partition& prt);

  std::vector<double> GatherIntegrals(double local_integral);

 private:
  int rank_;
  int world_size_;

  int errno_;

  Logger logger_;
};

////////////////////////////////////////////////////////////////////////////////

}  // namespace compute

////////////////////////////////////////////////////////////////////////////////

#endif  // _ROUTINE_HPP
