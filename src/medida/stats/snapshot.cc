//
// Copyright (c) 2012 Daniel Lundin
//

#include "medida/stats/snapshot.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <cassert>

namespace medida {
namespace stats {

static const double kMEDIAN_Q = 0.5;
static const double kP75_Q = 0.75;
static const double kP95_Q = 0.95;
static const double kP98_Q = 0.98;
static const double kP99_Q = 0.99;
static const double kP999_Q = 0.999;

class Snapshot::Impl {
 public:
  virtual std::size_t size() const = 0;
  virtual double getValue(double quantile) = 0;
  virtual double getMedian();
  virtual double get75thPercentile();
  virtual double get95thPercentile();
  virtual double get98thPercentile();
  virtual double get99thPercentile();
  virtual double get999thPercentile();
  virtual double sum() const { return 0; };
  virtual double min() const { return 0; };
  virtual double max() const { return 0; };
  virtual double variance() const { return 0; };
  virtual std::vector<double> getValues() const = 0;
};

class Snapshot::VectorImpl : public Snapshot::Impl {
 public:
  VectorImpl(const std::vector<double>& values);
  ~VectorImpl();
  std::size_t size() const override;
  double getValue(double quantile) override;
  std::vector<double> getValues() const override;
 private:
  std::vector<double> values_;
};


class Snapshot::CKMSImpl : public Snapshot::Impl {
 public:
  CKMSImpl(const CKMS& ckms);
  ~CKMSImpl();
  std::size_t size() const override;
  double getValue(double quantile) override;
  double sum() const override;
  double min() const override;
  double max() const override;
  double variance() const override;
  std::vector<double> getValues() const override;
 private:
  CKMS ckms_;
};


Snapshot::Snapshot(const std::vector<double>& values)
  : impl_ {new Snapshot::VectorImpl {values}} {
}

Snapshot::Snapshot(const CKMS& ckms)
  : impl_ {new Snapshot::CKMSImpl {ckms}} {
}

Snapshot::Snapshot(Snapshot&& other)
    : impl_ {std::move(other.impl_)} {
}

Snapshot::~Snapshot() {
}

void Snapshot::checkImpl() const
{
  if (!impl_)
  {
    throw std::runtime_error("Access to moved Snapshot::impl_");
  }
}

std::size_t Snapshot::size() const {
  checkImpl();
  return impl_->size();
}

double Snapshot::min() {
  checkImpl();
  return impl_->min();
}

double Snapshot::max() {
  checkImpl();
  return impl_->max();
}

double Snapshot::variance() {
  checkImpl();
  return impl_->variance();
}

double Snapshot::sum() {
  checkImpl();
  return impl_->sum();
}

std::vector<double> Snapshot::getValues() const {
  checkImpl();
  return impl_->getValues();
}


double Snapshot::getValue(double quantile) {
  checkImpl();
  return impl_->getValue(quantile);
}


double Snapshot::getMedian() {
  checkImpl();
  return impl_->getMedian();
}


double Snapshot::get75thPercentile() {
  checkImpl();
  return impl_->get75thPercentile();
}


double Snapshot::get95thPercentile() {
  checkImpl();
  return impl_->get95thPercentile();
}


double Snapshot::get98thPercentile() {
  checkImpl();
  return impl_->get98thPercentile();
}


double Snapshot::get99thPercentile() {
  checkImpl();
  return impl_->get99thPercentile();
}


double Snapshot::get999thPercentile() {
  checkImpl();
  return impl_->get999thPercentile();
}


// === Implementation ===


Snapshot::VectorImpl::VectorImpl(const std::vector<double>& values)
    : values_ (values) {
  std::sort(std::begin(this->values_), std::end(this->values_));
}


Snapshot::VectorImpl::~VectorImpl() {
}


std::size_t Snapshot::VectorImpl::size() const {
 return values_.size();
}


std::vector<double> Snapshot::VectorImpl::getValues() const {
  return values_;
}


double Snapshot::VectorImpl::getValue(double quantile)
{
    // Calculating a quantile is _mostly_ just about scaling the requested
    // quantile from the range it's given in [0.0, 1.0] to an index value in the
    // range of valid indices for the sorted data. Unfortunately there are two
    // complications:
    //
    //   1. If the scaled quantile doesn't land exactly on an integer value, you
    //      have to interpolate "somehow" between the values at ceiling and
    //      floor indices. It turns out there's little agreement in the world of
    //      stats about which form of interpolation is best or how to achieve
    //      it. R itself has 9 variants available, but the "most popular" (and
    //      its default) appears to be algorithm R7 from Hyndman and Fan (1996).
    //
    //   2. Even "textbook" algorithms like R7 are described using 1-based
    //      indexing, which makes it somewhat non-obvious to transcribe directly
    //      or even copy from other scientific languages (that do 1-based) into
    //      C++ 0-based indexing. So we have to try our own hand at implementing
    //      it "from intent" rather than copying code directly from elsewhere.
    //
    // We've tested this with enough test vectors from R to convince ourselves
    // it's a faithful implementation.
    //
    // https://www.rdocumentation.org/packages/stats/versions/3.6.2/topics/quantile
    // https://en.wikipedia.org/wiki/Quantile#Estimating_quantiles_from_a_sample

    if (quantile < 0.0 || quantile > 1.0)
    {
        throw std::invalid_argument("quantile is not in [0..1]");
    }

    if (values_.empty())
    {
        return 0.0;
    }

    // Step 1: define range of actually-allowed indexes: [0, max_idx]
    size_t max_idx = values_.size() - 1;

    // Step 2: calculate "ideal" fractional index (with 1.0 => max_idx).
    double ideal_index = quantile * max_idx;

    // Step 3: calculate ideal-index floor and integral low and hi indexes.
    double floor_ideal = std::floor(ideal_index);
    assert(floor_ideal >= 0.0);
    size_t lo_idx = static_cast<size_t>(floor_ideal);
    assert(lo_idx <= max_idx);
    size_t hi_idx = lo_idx + 1;

    // Step 4: if there's no upper sample to interpolate with, just return
    // the highest one.
    if (hi_idx > max_idx)
    {
        return values_.back();
    }

    // Step 5: return linear interpolation of elements at lo_idx and hi_idx.
    double delta = ideal_index - floor_ideal;
    assert(delta >= 0.0);
    assert(delta < 1.0);
    double lower = values_.at(lo_idx);
    double upper = values_.at(hi_idx);
    return lower + (delta * (upper - lower));
}

Snapshot::CKMSImpl::CKMSImpl(const CKMS& ckms)
    : ckms_ (ckms) {
}


Snapshot::CKMSImpl::~CKMSImpl() {
}


std::size_t Snapshot::CKMSImpl::size() const {
    return ckms_.count();
}


std::vector<double> Snapshot::CKMSImpl::getValues() const {
    throw std::runtime_error("Can't return the values since ckms doesn't have them");
}

double Snapshot::CKMSImpl::variance() const {
    return ckms_.variance();
}

double Snapshot::CKMSImpl::sum() const {
    return ckms_.sum();
}

double Snapshot::CKMSImpl::max() const {
    return ckms_.max();
}

double Snapshot::CKMSImpl::min() const {
    return ckms_.min();
}

double Snapshot::CKMSImpl::getValue(double quantile) {
    return ckms_.get(quantile);
}

double Snapshot::Impl::getMedian() {
  return getValue(kMEDIAN_Q);
}


double Snapshot::Impl::get75thPercentile() {
  return getValue(kP75_Q);
}


double Snapshot::Impl::get95thPercentile() {
  return getValue(kP95_Q);
}


double Snapshot::Impl::get98thPercentile() {
  return getValue(kP98_Q);
}


double Snapshot::Impl::get99thPercentile() {
  return getValue(kP99_Q);
}


double Snapshot::Impl::get999thPercentile() {
  return getValue(kP999_Q);
}


} // namespace stats
} // namespace medida
