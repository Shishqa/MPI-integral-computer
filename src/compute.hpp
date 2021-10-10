#ifndef _INTEGRAL_H
#define _INTEGRAL_H

#include <vector>

#include <cstddef>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////

namespace compute {

////////////////////////////////////////////////////////////////////////////////

/* Real-number one-argument function (R -> R) */
using RealFunc = double (*)(double x);

/* Sector on the Real number line */
struct Sector {
  double from, to;
};

class Partition {
 public:
  Partition(const Sector& sector, size_t n_parts)
    : sector_(sector), n_parts_(n_parts)
  { }

  Partition() = default;

  double Step() const {
    return (sector_.to - sector_.from) / n_parts_;
  }

  double From() const {
    return sector_.from;
  }

  double To() const {
    return sector_.to;
  }

  size_t StepsCount() const {
    return n_parts_;
  }

  std::vector<Partition> Split(size_t n_subparts) const;

 private:
  Sector sector_;
  size_t n_parts_;
};

double ComputeIntegral(RealFunc func, const Partition& prt, size_t n_threads);

////////////////////////////////////////////////////////////////////////////////

}  // namespace integral

////////////////////////////////////////////////////////////////////////////////

#endif  // _INTEGRAL_HPP
