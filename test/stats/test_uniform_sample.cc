//
// Copyright (c) 2012 Daniel Lundin AB
//


#include "medida/stats/uniform_sample.h"

#include <gtest/gtest.h>

using namespace medida::stats;

TEST(UniformSampleTest, aSampleOf100OutOf1000Elements) {
  UniformSample sample {100};

  for (auto i = 0; i < 1234; i++) {
    sample.Update(i);
  }

  EXPECT_EQ(100, sample.size());

  auto vals = sample.MakeSnapshot().getValues();
  EXPECT_EQ(100, vals.size());

  for (auto& v : vals) {
    EXPECT_LT(v, 1234);
    EXPECT_GE(v, 0);
  }
}

TEST(UniformSampleTest, clear) {
  UniformSample sample {100};
  for (auto i = 0; i < 1234; i++) {
    sample.Update(i);
  }

  EXPECT_EQ(100, sample.size());

  sample.Clear();

  EXPECT_EQ(0, sample.size());
}

