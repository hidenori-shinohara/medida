#include "medida/stats/ckms.h"
#include <gtest/gtest.h>

using namespace medida::stats;

TEST(CKMSTest, aCKMSAddHundredOnes) {
  std::vector<CKMS::Quantile> v;
  auto ckms = CKMS(v);
  for (int i = 0; i < 100; i++) {
      ckms.insert(1);
  }
  EXPECT_NEAR(1, ckms.get(0.5), 1e-6);
  EXPECT_NEAR(1, ckms.get(0.99), 1e-6);
  EXPECT_NEAR(1, ckms.get(1), 1e-6);
}

TEST(CKMSTest, aCKMSAddOneToHundred) {
  std::vector<CKMS::Quantile> v({{0.5, 0.001}, {0.99, 0.001}, {1, 0}});
  auto ckms = CKMS(v);
  for (int i = 1; i <= 100; i++) {
      ckms.insert(i);
  }
  EXPECT_NEAR(50, ckms.get(0.5), 1e-6);
  EXPECT_NEAR(99, ckms.get(0.99), 1e-6);
  EXPECT_NEAR(100, ckms.get(1), 1e-6);
}
