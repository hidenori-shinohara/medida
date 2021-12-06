// Copyright 2021 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

namespace medida {
namespace stats {


// TODO(hidenori): This file and the corresponding source file
// is where the CKMS quantile estimation algorithm should be.
class Ckms {
    public:
      // TODO(hidenori): Add more functions if necessary.
      double count() const;
      double estimateQuantile(double q);
};

} // namespace stats
} // namespace medida
