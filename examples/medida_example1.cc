#include "medida/medida.h"
#include <memory>
#include <iostream>
#include <sstream>

namespace std {
    template<typename T, typename... Args>
        std::unique_ptr<T> make_unique(Args&&... args) {
            return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        }
}

int main() {
    std::unique_ptr<medida::MetricsRegistry> mMetrics = std::make_unique<medida::MetricsRegistry>();
    medida::Timer& mLedgerClose = mMetrics->NewTimer({"ledger", "ledger", "close"});
    auto ledgerTime = mLedgerClose.TimeScope();

    ledgerTime.Stop();

    medida::reporting::JsonReporter reporter(*mMetrics);

    for (auto& kv : mMetrics->GetAllMetrics())
    {
        auto name = kv.first.ToString();
        auto metric = kv.second;
        metric->Process(reporter);
    }

    std::cout << reporter.Report() << std::endl;
    return 0;
}