// Copyright 2021 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0
//


// TODO(hidenori): Properly take care of the license stuff.
#include <array>
#include <cstddef>
#include <functional>
#include <vector>

namespace medida {
namespace stats {

class CKMS {
 public:
  struct Quantile {
    Quantile(double quantile, double error);

    double quantile;
    double error;
    double u;
    double v;
  };

 private:
  struct Item {
    double value;
    int g;
    int delta;

    Item(double value, int lower_delta, int delta);
  };

 public:
  explicit CKMS(const std::vector<Quantile>& quantiles);

  void insert(double value);
  double get(double q);
  void reset();
  std::size_t count() const;

 private:
  double allowableError(int rank);
  bool insertBatch();
  void compress();

 private:
  const std::reference_wrapper<const std::vector<Quantile>> quantiles_;

  std::size_t count_;
  std::vector<Item> sample_;
  std::array<double, 500> buffer_;
  std::size_t buffer_count_;
};

} // namespace stats
} // namespace medida
