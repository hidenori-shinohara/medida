#include "medida/stats/ckms.h"
#include <gtest/gtest.h>

using namespace medida::stats;

TEST(CKMSTest, aCKMSAddHundredOnes) {
  auto ckms = CKMS({{0.99, 0.001}});
  for (int i = 0; i < 100; i++) {
      ckms.insert(1);
  }
  EXPECT_NEAR(1, ckms.get(0.5), 1e-6);
  EXPECT_NEAR(1, ckms.get(0.99), 1e-6);
  EXPECT_NEAR(1, ckms.get(1), 1e-6);
}

TEST(CKMSTest, aCKMSAddZeroToHundred) {
  auto ckms = CKMS({{0.99, 0.001}});
  for (int i = 0; i <= 100; i++) {
      ckms.insert(i);
  }
  EXPECT_NEAR(50, ckms.get(0.5), 1e-6);
  EXPECT_NEAR(99, ckms.get(0.99), 1e-6);
  EXPECT_NEAR(100, ckms.get(1), 1e-6);
}
