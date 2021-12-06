#include "medida/stats/ckms.h"
#include <gtest/gtest.h>

using namespace medida::stats;

TEST(CkmsTest, aCkmsAddHundredOnes) {
  auto ckms = Ckms();
  for (int i = 0; i < 100; i++) {
      ckms.add(1);
  }
  EXPECT_NEAR(1, ckms.estimateQuantile(0.5), 1e-6);
  EXPECT_NEAR(1, ckms.estimateQuantile(0.99), 1e-6);
  EXPECT_NEAR(1, ckms.estimateQuantile(1), 1e-6);
}

TEST(CkmsTest, aCkmsAddZeroToHundred) {
  auto ckms = Ckms();
  for (int i = 0; i <= 100; i++) {
      ckms.add(i);
  }
  EXPECT_NEAR(50, ckms.estimateQuantile(0.5), 1e-6);
  EXPECT_NEAR(99, ckms.estimateQuantile(0.99), 1e-6);
  EXPECT_NEAR(100, ckms.estimateQuantile(1), 1e-6);
}
