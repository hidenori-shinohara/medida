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

class CKMSSample::Impl {
 public:
  Impl();
  ~Impl();
  void Clear();
  std::uint64_t size() const;
  void Update(std::int64_t value);
  void Update(std::int64_t value, Clock::time_point timestamp);
  Snapshot MakeSnapshot() const;
 private:
  CKMS mCKMS;
};

CKMSSample::CKMSSample() : impl_ {new CKMSSample::Impl {}} {
}


CKMSSample::~CKMSSample() {
}


void CKMSSample::Clear() {
  impl_->Clear();
}


std::uint64_t CKMSSample::size() const {
  return impl_->size();
}


void CKMSSample::Update(std::int64_t value) {
  impl_->Update(value);
}


void CKMSSample::Update(std::int64_t value, Clock::time_point timestamp) {
  impl_->Update(value, timestamp);
}


Snapshot CKMSSample::MakeSnapshot() const {
  return impl_->MakeSnapshot();
}


// === Implementation ===


// TODO(hidenori): Think about what is the appropriate default accuracy.
// For now, I'm putting {99th percentile, 0.1% error}.
CKMSSample::Impl::Impl() :
    mCKMS({})
{
    Clear();
}


CKMSSample::Impl::~Impl() {
}


void CKMSSample::Impl::Clear() {
}


std::uint64_t CKMSSample::Impl::size() const {
    // TODO(hidenori) This is a placeholder.
    return 0;
}


void CKMSSample::Impl::Update(std::int64_t value) {
  Update(value, Clock::now());
}


void CKMSSample::Impl::Update(std::int64_t value, Clock::time_point timestamp) {
    // TODO(hidenori): This is where I need to pick the right CKMS to add this value
    // and possibly swap the two CKMS's.
}


Snapshot CKMSSample::Impl::MakeSnapshot() const {
    // TODO(hidenori): Decide if this is the right value to return.
    return {mCKMS};
}


} // namespace stats
} // namespace medida
