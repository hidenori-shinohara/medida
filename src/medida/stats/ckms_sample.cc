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

// TODO(hidenori)
// Figure out how to properly update the window size.
class CKMSSample::Impl {
 public:
  Impl(std::chrono::seconds = std::chrono::seconds(30));
  ~Impl();
  void Clear();
  std::uint64_t size() const;
  void Update(std::int64_t value);
  void Update(std::int64_t value, Clock::time_point timestamp);
  Snapshot MakeSnapshot();
 private:
  void shift();
  std::shared_ptr<CKMS> mPrev, mCur;
  Clock::time_point mPrevStartingPoint;
  std::chrono::seconds mWindowSize;
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

Clock::time_point getCurrentWindowStartingPoint(Clock::time_point time, std::chrono::seconds windowSize)
{
    return time - (std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()) % windowSize);
}

// TODO(hidenori): Think about what is the appropriate default accuracy.
// For now, I'm putting {99th percentile, 0.1% error}.
CKMSSample::Impl::Impl(std::chrono::seconds windowSize) :
    mPrev({}),
    mCur({}),
    mPrevStartingPoint(getCurrentWindowStartingPoint(medida::Clock::now(), windowSize) - windowSize),
    mWindowSize(windowSize)
{
    Clear();
}


CKMSSample::Impl::~Impl() {
}


void CKMSSample::Impl::Clear() {
}


std::uint64_t CKMSSample::Impl::size() const {
    return mPrev->count();
}


void CKMSSample::Impl::Update(std::int64_t value) {
  Update(value, Clock::now());
}


void CKMSSample::Impl::shift() {
    auto curStartingPoint = getCurrentWindowStartingPoint(medida::Clock::now(), mWindowSize);
    if (mPrevStartingPoint + mWindowSize == curStartingPoint)
    {
        return;
    }
    else if (mPrevStartingPoint + 2 * mWindowSize == curStartingPoint)
    {
        // mCur should be the next mPrev.
        std::swap(mPrev, mCur);
        mCur->reset();
    }
    else if (mPrevStartingPoint + 2 * mWindowSize < curStartingPoint)
    {
        // We haven't had any input for long enough that both mPrev and mCur should be empty.
        mPrev->reset();
        mCur->reset();
        mPrevStartingPoint = curStartingPoint - mWindowSize;
    }
    else
    {
        throw std::invalid_argument("mPrevStartingPoint has an unexpected value");
    }
}

void CKMSSample::Impl::Update(std::int64_t value, Clock::time_point timestamp) {
    shift();
    auto now = medida::Clock::now();
    if (timestamp < now && timestamp < getCurrentWindowStartingPoint(now, mWindowSize))
    {
        // Inser this value only if timestamp lies in the current time window.
        mCur->insert(value);
    }
}


Snapshot CKMSSample::Impl::MakeSnapshot() {
    shift();
    return {*mPrev};
}


} // namespace stats
} // namespace medida
