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

  // We should only keep 30 seconds of data.
  EXPECT_EQ(30, sample.size(t));

  auto snapshot = sample.MakeSnapshot(t);

  EXPECT_EQ(100, snapshot.getValue(0.5));
  EXPECT_EQ(100, snapshot.getValue(0.99));
  EXPECT_EQ(100, snapshot.getValue(1));
}

TEST(CKMSSampleTest, aThreeDifferentValues) {
  CKMSSample sample;

  auto t = medida::Clock::now();
  for (auto i = 0; i < 300; i++) {
    sample.Update(i % 3, t);
    t += std::chrono::seconds(1);
  }

  // We should only keep 30 seconds of data.
  EXPECT_EQ(30, sample.size(t));

  auto snapshot = sample.MakeSnapshot(t);

  EXPECT_EQ(1, snapshot.getValue(0.5));
  EXPECT_EQ(2, snapshot.getValue(0.99));
  EXPECT_EQ(2, snapshot.getValue(1));
}
