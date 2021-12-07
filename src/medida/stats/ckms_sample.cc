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
#include <cassert>

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
  std::uint64_t size(Clock::time_point timestamp) const;
  void Update(std::int64_t value);
  void Update(std::int64_t value, Clock::time_point timestamp);
  Snapshot MakeSnapshot() const;
  Snapshot MakeSnapshot(Clock::time_point timestamp) const;
 private:
  std::shared_ptr<CKMS> mPrev, mCur;
  mutable Clock::time_point mLastAssertedTime;
  Clock::time_point mCurrentWindowBegin;
  std::chrono::seconds mWindowSize;
  Clock::time_point getCurrentWindowStartingPoint(Clock::time_point time) const;
  void assertValidTime(Clock::time_point const& timestamp) const;
  bool isInCurrentWindow(Clock::time_point const& timestamp) const;
  bool isInNextWindow(Clock::time_point const& timestamp) const;
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

std::uint64_t CKMSSample::size(Clock::time_point timestamp) const {
  return impl_->size(timestamp);
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

Snapshot CKMSSample::MakeSnapshot(Clock::time_point timestamp) const {
  return impl_->MakeSnapshot(timestamp);
}


// === Implementation ===

Clock::time_point CKMSSample::Impl::getCurrentWindowStartingPoint(Clock::time_point time) const
{
    return time - (std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()) % mWindowSize);
}

void CKMSSample::Impl::assertValidTime(Clock::time_point const& timestamp) const
{
    assert(mLastAssertedTime <= timestamp);
    mLastAssertedTime = timestamp;
}

bool CKMSSample::Impl::isInCurrentWindow(Clock::time_point const& timestamp) const
{
    return mCurrentWindowBegin <= timestamp && timestamp <= mCurrentWindowBegin + mWindowSize;
}

bool CKMSSample::Impl::isInNextWindow(Clock::time_point const& timestamp) const
{
    return mCurrentWindowBegin + mWindowSize <= timestamp && timestamp <= mCurrentWindowBegin + 2 * mWindowSize;
}

// TODO(hidenori): Think about what is the appropriate default accuracy.
// For now, I'm putting {99th percentile, 0.1% error}.
CKMSSample::Impl::Impl(std::chrono::seconds windowSize) :
    mPrev(std::make_shared<CKMS>(CKMS())),
    mCur(std::make_shared<CKMS>(CKMS())),
    mLastAssertedTime(),
    mCurrentWindowBegin(),
    mWindowSize(windowSize)
{
    Clear();
}


CKMSSample::Impl::~Impl() {
}


void CKMSSample::Impl::Clear() {
// TODO(hidenori): clear these values
//    mPrev(),
//    mCur({}),
//    mLastAssertedTime(),
//    mCurrentWindowBegin(),
}


std::uint64_t CKMSSample::Impl::size(Clock::time_point timestamp) const {
    return MakeSnapshot(timestamp).size();
}

std::uint64_t CKMSSample::Impl::size() const {
    return size(Clock::now());
}


void CKMSSample::Impl::Update(std::int64_t value) {
  Update(value, Clock::now());
}


void CKMSSample::Impl::Update(std::int64_t value, Clock::time_point timestamp) {
    assertValidTime(timestamp);
    if (!isInCurrentWindow(timestamp))
    {
        // Enough time has passed, and the current window is no longer current.
        // We need to shift it.

        if (isInNextWindow(timestamp))
        {
            // The current window becomes the previous one.
            mPrev.swap(mCur);
            mCur->reset();
            mCurrentWindowBegin += mWindowSize;
        }
        else
        {
            // We haven't had any input for long enough that both mPrev and mCur should be empty.
            mPrev->reset();
            mCur->reset();
            mCurrentWindowBegin = getCurrentWindowStartingPoint(timestamp);
        }
    }
    mCur->insert(value);
}


Snapshot CKMSSample::Impl::MakeSnapshot(Clock::time_point timestamp) const {
    assertValidTime(timestamp);
    if (isInCurrentWindow(timestamp)) {
        return {*mPrev};
    } else if (isInNextWindow(timestamp)) {
        return {*mCur};
    } else {
        return {CKMS()};
    }
}

Snapshot CKMSSample::Impl::MakeSnapshot() const {
    return MakeSnapshot(Clock::now());
}


} // namespace stats
} // namespace medida
