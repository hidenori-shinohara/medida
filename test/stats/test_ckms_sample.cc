#include "medida/stats/ckms_sample.h"

#include <gtest/gtest.h>

using namespace medida::stats;

TEST(CKMSSampleTest, aSameValueEverySecond) {
  CKMSSample sample;

  auto t = medida::Clock::now();
  for (auto i = 0; i < 300; i++) {
    sample.Update(100, t);
    t += std::chrono::seconds(1);
  }

  EXPECT_EQ(300, sample.size());

  auto snapshot = sample.MakeSnapshot();

  EXPECT_EQ(100, snapshot.getValue(0.5));
  EXPECT_EQ(100, snapshot.getValue(0.99));
  EXPECT_EQ(100, snapshot.getValue(1));
}
