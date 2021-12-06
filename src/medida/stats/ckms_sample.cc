// Copyright 2021 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include "medida/stats/ckms_sample.h"

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

class CkmsSample::Impl {
 public:
  Impl();
  ~Impl();
  void Clear();
  std::uint64_t size() const;
  void Update(std::int64_t value);
  void Update(std::int64_t value, Clock::time_point timestamp);
  Snapshot MakeSnapshot() const;
 private:
  Ckms mCkms;
};

CkmsSample::CkmsSample() : impl_ {new CkmsSample::Impl {}} {
}


CkmsSample::~CkmsSample() {
}


void CkmsSample::Clear() {
  impl_->Clear();
}


std::uint64_t CkmsSample::size() const {
  return impl_->size();
}


void CkmsSample::Update(std::int64_t value) {
  impl_->Update(value);
}


void CkmsSample::Update(std::int64_t value, Clock::time_point timestamp) {
  impl_->Update(value, timestamp);
}


Snapshot CkmsSample::MakeSnapshot() const {
  return impl_->MakeSnapshot();
}


// === Implementation ===


CkmsSample::Impl::Impl() {
    Clear();
}


CkmsSample::Impl::~Impl() {
}


void CkmsSample::Impl::Clear() {
}


std::uint64_t CkmsSample::Impl::size() const {
    // TODO(hidenori) This is a placeholder.
    return 0;
}


void CkmsSample::Impl::Update(std::int64_t value) {
  Update(value, Clock::now());
}


void CkmsSample::Impl::Update(std::int64_t value, Clock::time_point timestamp) {
    // TODO(hidenori): This is where I need to pick the right CKMS to add this value
    // and possibly swap the two CKMS's.
}


Snapshot CkmsSample::Impl::MakeSnapshot() const {
    // TODO(hidenori): Decide if this is the right value to return.
    return {mCkms};
}


} // namespace stats
} // namespace medida
