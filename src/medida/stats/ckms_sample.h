// Copyright 2021 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#ifndef MEDIDA_CKMS_SAMPLE_H_
#define MEDIDA_CKMS_SAMPLE_H_

#include <cstdint>
#include <memory>

#include "medida/types.h"
#include "medida/stats/sample.h"
#include "medida/stats/snapshot.h"

namespace medida {
namespace stats {

class CKMSSample : public Sample {
 public:
  CKMSSample();
  ~CKMSSample();
  virtual void Clear();
  virtual std::uint64_t size() const;
  virtual void Update(std::int64_t value);
  virtual void Update(std::int64_t value, Clock::time_point timestamp);
  virtual Snapshot MakeSnapshot() const;
 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace stats
} // namespace medida

#endif // MEDIDA_CKMS_SAMPLE_H_
