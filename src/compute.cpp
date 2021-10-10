#include <omp.h>
#include <cassert>

#include "compute.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace compute {

////////////////////////////////////////////////////////////////////////////////

std::vector<Partition> Partition::Split(size_t n_subparts) const
{
  std::vector<Partition> sub_prt;
  sub_prt.reserve(n_subparts);

  const size_t steps_per_part = (n_parts_ + n_subparts - 1) / n_subparts;
  const double len_per_part = steps_per_part * Step();

  double curr_begin = sector_.from;
  for (size_t i = 0; i < n_subparts; ++i, curr_begin += len_per_part) {
    const double curr_end = fmin(curr_begin + len_per_part, sector_.to);
    const size_t n_steps = (i == n_subparts - 1 ? n_parts_ - i * steps_per_part
                                                : steps_per_part);

    sub_prt.emplace_back(Sector{curr_begin, curr_end}, n_steps);
  }

  return sub_prt;
}

/* -------------------------------------------------------------------------- */

static double ComputeTrapezoidArea(double base_a, double base_b, double h)
{
  return (base_a + base_b) * h / 2.0;
}

double ComputeIntegral(RealFunc func, const Partition& prt)
{
  const double dx = prt.Step();

  double area = 0.0;
  double x_i  = prt.From();

  for (size_t step = 0; step < prt.StepsCount(); ++step, x_i += dx) {
    area += ComputeTrapezoidArea(func(x_i), func(x_i + dx), dx);
  }

  return area;
}

double ComputeIntegralOMP(RealFunc func, const Partition& prt, size_t n_threads)
{
  if (n_threads <= 1) {
    return ComputeIntegral(func, prt);
  }

  const double dx = prt.Step();
  const size_t steps_count = prt.StepsCount();
  const double from = prt.From();

  double area = 0.0;
  double x_i  = 0.0;
  size_t step = 0;

  omp_set_num_threads(n_threads);

#pragma omp parallel default(shared)
{
    assert(n_threads == omp_get_num_threads());
#pragma omp for schedule(static) reduction(+:area) nowait private(step, x_i)
    for (step = 0; step < steps_count; ++step) {
      x_i = from + step * dx;
      area += ComputeTrapezoidArea(func(x_i), func(x_i + dx), dx);
    }
  }

  return area;
}

////////////////////////////////////////////////////////////////////////////////

}  // namespace compute
