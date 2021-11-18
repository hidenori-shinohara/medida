//
// Copyright (c) 2012 Daniel Lundin
//

#ifndef MEDIDA_METRICS_SNAPSHOT_H_
#define MEDIDA_METRICS_SNAPSHOT_H_

#include "medida/stats/tdigest.h"

#include <memory>
#include <vector>

namespace medida {
namespace stats {

class Snapshot {
 public:
  Snapshot(const std::vector<double>& values);
  Snapshot(const TDigest& tDigest);
  ~Snapshot();
  Snapshot(Snapshot const&) = delete;
  Snapshot& operator=(Snapshot const&) = delete;
  Snapshot(Snapshot&&);
  std::size_t size() const;
  double getValue(double quantile) const;
  double getMedian() const;
  double get75thPercentile() const;
  double get95thPercentile() const;
  double get98thPercentile() const;
  double get99thPercentile() const;
  double get999thPercentile() const;
  std::vector<double> getValues() const;
  class Impl;
  class VectorImpl;
  class TDigestImpl;
 private:
  void checkImpl() const;
  std::unique_ptr<Impl> impl_;
};


} // namespace stats
} // namespace medida

#endif // MEDIDA_METRICS_SNAPSHOT_H_
