#include "medida/medida.h"
#include <memory>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

namespace std {
    template<typename T, typename... Args>
        std::unique_ptr<T> make_unique(Args&&... args) {
            return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        }
}

int main() {
    std::unique_ptr<medida::MetricsRegistry> mMetrics = std::make_unique<medida::MetricsRegistry>();
    medida::Timer& mLedgerClose = mMetrics->NewTimer({"ledger", "ledger", "close"});
    for (int t = 100; t <= 1000; t += 100) {
        auto ledgerTime = mLedgerClose.TimeScope();
        std::this_thread::sleep_for(std::chrono::milliseconds(t));
        ledgerTime.Stop();
    }

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