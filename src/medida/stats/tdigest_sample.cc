// Copyright 2021 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include "medida/stats/tdigest_sample.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <functional>
#include <map>
#include <mutex>
#include <random>

namespace medida {
namespace stats {

class TDigestSample::Impl {
 public:
  Impl();
  ~Impl();
  void Clear();
  std::uint64_t size() const;
  void Update(std::int64_t value);
  void Update(std::int64_t value, Clock::time_point timestamp);
  Snapshot MakeSnapshot() const;
 private:
  TDigest mTDigest;
};

TDigestSample::TDigestSample() : impl_ {new TDigestSample::Impl {}} {
}


TDigestSample::~TDigestSample() {
}


void TDigestSample::Clear() {
  impl_->Clear();
}


std::uint64_t TDigestSample::size() const {
  return impl_->size();
}


void TDigestSample::Update(std::int64_t value) {
  impl_->Update(value);
}


void TDigestSample::Update(std::int64_t value, Clock::time_point timestamp) {
  impl_->Update(value, timestamp);
}


Snapshot TDigestSample::MakeSnapshot() const {
  return impl_->MakeSnapshot();
}


// === Implementation ===


TDigestSample::Impl::Impl() {
    Clear();
}


TDigestSample::Impl::~Impl() {
}


void TDigestSample::Impl::Clear() {
}


std::uint64_t TDigestSample::Impl::size() const {
}


void TDigestSample::Impl::Update(std::int64_t value) {
  Update(value, Clock::now());
}


void TDigestSample::Impl::Update(std::int64_t value, Clock::time_point timestamp) {
}


Snapshot TDigestSample::Impl::MakeSnapshot() const {
    return {mTDigest};
}


} // namespace stats
} // namespace medida
