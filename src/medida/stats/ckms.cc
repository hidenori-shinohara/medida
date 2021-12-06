#include "medida/stats/ckms.h"

namespace medida {
namespace stats {

double Ckms::count() const {
    // TODO(hidenori): This is obviously a placeholder.
    return 0;
}

double Ckms::estimateQuantile(double q) {
    // TODO(hidenori): This is obviously a placeholder.
    if (q < 0 || 1 < q) {
        return -1;
    }
    return 0;
}

} // namespace stats
} // namespace medida
