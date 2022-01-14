#include "medida/stats/ckms.h"
#include <gtest/gtest.h>
#include <algorithm>
#include <random>

using namespace medida::stats;

TEST(CKMSTest, aCKMSAddHundredOnes) {
  std::vector<CKMS::Quantile> v({{0.5, 0.001}, {0.99, 0.001}, {1, 0}});
  auto ckms = CKMS(v);
  for (int i = 0; i < 100; i++) {
      ckms.insert(1);
  }
  EXPECT_NEAR(1, ckms.get(0.5), 1e-6);
  EXPECT_NEAR(1, ckms.get(0.99), 1e-6);
  EXPECT_NEAR(1, ckms.get(1), 1e-6);
}

TEST(CKMSTest, aCKMSAddOneToHundredThounsand) {
  // 0.1% error
  //
  // E.g., when guessing P99, it returns a value between
  // - P(1 - 0.001) * 99 = P98.901, and
  // - P(1 + 0.001) * 99 = P99.099
  //
  // See the definition of \epsilon-approximate in
  // http://dimacs.rutgers.edu/~graham/pubs/papers/bquant-icde.pdf
  double const error = 0.001;


  auto const percentiles = {0.5, 0.75, 0.9, 0.99};
  std::vector<CKMS::Quantile> v;
  v.reserve(percentiles.size());
  for (auto const q: percentiles) {
    v.push_back({q, error});
  }

  auto ckms = CKMS(v);

  int const count = 100 * 1000;
  for (int i = 1; i <= count; i++) {
      ckms.insert(i);
  }

  for (auto const q: percentiles) {
      EXPECT_LE((1 - error) * q * count, ckms.get(q));
      EXPECT_GE((1 + error) * q * count, ckms.get(q));
  }
}

TEST(CKMSTest, aCKMSUniform) {
  double const error = 0.001;
  auto const percentiles = {0.5, 0.75, 0.9, 0.99};
  std::vector<CKMS::Quantile> v;
  v.reserve(percentiles.size());
  for (auto const q: percentiles) {
    v.push_back({q, error});
  }

  auto ckms = CKMS(v);

  srand(time(NULL));
  int const count = 100 * 1000;
  std::vector<int> values;
  values.reserve(count);
  for (int i = 1; i <= count; i++) {
      auto x = rand();
      values.push_back(x);
      ckms.insert(x);
  }

  std::sort(values.begin(), values.end());
  for (auto const q: percentiles) {
      EXPECT_LE(values[int((1 - error) * q * count)], ckms.get(q));
      EXPECT_GE(values[int((1 + error) * q * count)], ckms.get(q));
  }
}

TEST(CKMSTest, aCKMSGamma) {
  double const error = 0.001;
  auto const percentiles = {0.5, 0.75, 0.9, 0.99};
  std::vector<CKMS::Quantile> v;
  v.reserve(percentiles.size());
  for (auto const q: percentiles) {
    v.push_back({q, error});
  }

  auto ckms = CKMS(v);

  int const count = 100 * 1000;
  std::vector<double> values;
  values.reserve(count);

  // 0 = seed
  std::mt19937 gen(0);

  // A gamma distribution with alpha=20, and beta=100
  // gives a bell curve with the top ~2000 between ~800 and ~400.
  std::gamma_distribution<double> d(20, 100);
  for (int i = 0; i < count; i++) {
      auto x = d(gen);
      values.push_back(x);
      ckms.insert(x);
  }

  std::sort(values.begin(), values.end());
  for (auto const q: percentiles) {
      EXPECT_LE(values[int((1 - error) * q * count)], ckms.get(q));
      EXPECT_GE(values[int((1 + error) * q * count)], ckms.get(q));
  }
}

TEST(CKMSTest, aCKMSPercentileOrder) {
  // This test case suggests that the order of inputs rarely matters.
  // Testing the behavior with {1, 2, ..., 3} with shuffling
  // Shuffle #0: P50 =  1, P75 =  2, P90 =  2, P99 =  2, P999 =  2
  // Shuffle #1: P50 =  1, P75 =  2, P90 =  2, P99 =  2, P999 =  2
  // Shuffle #2: P50 =  1, P75 =  2, P90 =  2, P99 =  2, P999 =  2
  // Shuffle #3: P50 =  1, P75 =  2, P90 =  2, P99 =  2, P999 =  2
  // Shuffle #4: P50 =  1, P75 =  2, P90 =  2, P99 =  2, P999 =  2
  // Shuffle #5: P50 =  1, P75 =  2, P90 =  2, P99 =  2, P999 =  2
  // Shuffle #6: P50 =  1, P75 =  2, P90 =  2, P99 =  2, P999 =  2
  // Shuffle #7: P50 =  1, P75 =  2, P90 =  2, P99 =  2, P999 =  2
  // Shuffle #8: P50 =  1, P75 =  2, P90 =  2, P99 =  2, P999 =  2
  // Shuffle #9: P50 =  1, P75 =  2, P90 =  2, P99 =  2, P999 =  2
  // Testing the behavior with {1, 2, ..., 10} with shuffling
  // Shuffle #0: P50 =  4, P75 =  6, P90 =  8, P99 =  8, P999 =  8
  // Shuffle #1: P50 =  4, P75 =  7, P90 =  8, P99 =  8, P999 =  8
  // Shuffle #2: P50 =  4, P75 =  7, P90 =  8, P99 =  8, P999 =  8
  // Shuffle #3: P50 =  5, P75 =  7, P90 =  8, P99 =  8, P999 =  8
  // Shuffle #4: P50 =  4, P75 =  6, P90 =  9, P99 =  9, P999 =  9
  // Shuffle #5: P50 =  5, P75 =  7, P90 =  9, P99 =  9, P999 =  9
  // Shuffle #6: P50 =  4, P75 =  7, P90 =  8, P99 =  8, P999 =  8
  // Shuffle #7: P50 =  4, P75 =  6, P90 =  9, P99 =  9, P999 =  9
  // Shuffle #8: P50 =  4, P75 =  6, P90 =  9, P99 =  9, P999 =  9
  // Shuffle #9: P50 =  4, P75 =  7, P90 =  8, P99 =  8, P999 =  8
  // Testing the behavior with {1, 2, ..., 20} with shuffling
  // Shuffle #0: P50 =  9, P75 = 14, P90 = 17, P99 = 18, P999 = 18
  // Shuffle #1: P50 =  9, P75 = 14, P90 = 17, P99 = 19, P999 = 19
  // Shuffle #2: P50 =  9, P75 = 14, P90 = 17, P99 = 18, P999 = 18
  // Shuffle #3: P50 =  9, P75 = 14, P90 = 18, P99 = 19, P999 = 19
  // Shuffle #4: P50 =  9, P75 = 15, P90 = 18, P99 = 19, P999 = 19
  // Shuffle #5: P50 =  9, P75 = 15, P90 = 17, P99 = 18, P999 = 18
  // Shuffle #6: P50 =  9, P75 = 14, P90 = 18, P99 = 19, P999 = 19
  // Shuffle #7: P50 = 10, P75 = 14, P90 = 18, P99 = 18, P999 = 18
  // Shuffle #8: P50 =  9, P75 = 14, P90 = 17, P99 = 19, P999 = 19
  // Shuffle #9: P50 =  9, P75 = 14, P90 = 17, P99 = 19, P999 = 19
  // Testing the behavior with {1, 2, ..., 100} with shuffling
  // Shuffle #0: P50 = 49, P75 = 74, P90 = 89, P99 = 98, P999 = 98
  // Shuffle #1: P50 = 49, P75 = 74, P90 = 89, P99 = 99, P999 = 99
  // Shuffle #2: P50 = 49, P75 = 74, P90 = 89, P99 = 98, P999 = 98
  // Shuffle #3: P50 = 49, P75 = 74, P90 = 89, P99 = 99, P999 = 99
  // Shuffle #4: P50 = 49, P75 = 74, P90 = 89, P99 = 99, P999 = 99
  // Shuffle #5: P50 = 49, P75 = 74, P90 = 89, P99 = 99, P999 = 99
  // Shuffle #6: P50 = 49, P75 = 74, P90 = 89, P99 = 99, P999 = 99
  // Shuffle #7: P50 = 49, P75 = 74, P90 = 89, P99 = 99, P999 = 99
  // Shuffle #8: P50 = 49, P75 = 74, P90 = 89, P99 = 98, P999 = 98
  // Shuffle #9: P50 = 49, P75 = 74, P90 = 89, P99 = 98, P999 = 98
  // Testing the behavior with {1, 2, ..., 1000} with shuffling
  // Shuffle #0: P50 = 500, P75 = 750, P90 = 899, P99 = 989, P999 = 998
  // Shuffle #1: P50 = 499, P75 = 750, P90 = 897, P99 = 988, P999 = 997
  // Shuffle #2: P50 = 499, P75 = 749, P90 = 900, P99 = 989, P999 = 998
  // Shuffle #3: P50 = 500, P75 = 748, P90 = 897, P99 = 989, P999 = 998
  // Shuffle #4: P50 = 500, P75 = 749, P90 = 898, P99 = 989, P999 = 998
  // Shuffle #5: P50 = 499, P75 = 748, P90 = 897, P99 = 989, P999 = 997
  // Shuffle #6: P50 = 500, P75 = 749, P90 = 898, P99 = 989, P999 = 998
  // Shuffle #7: P50 = 500, P75 = 749, P90 = 897, P99 = 989, P999 = 999
  // Shuffle #8: P50 = 500, P75 = 750, P90 = 897, P99 = 988, P999 = 998
  // Shuffle #9: P50 = 499, P75 = 749, P90 = 897, P99 = 988, P999 = 997
  // Testing the behavior with {1, 2, ..., 10000} with shuffling
  // Shuffle #0: P50 = 4998, P75 = 7498, P90 = 8994, P99 = 9897, P999 = 9987
  // Shuffle #1: P50 = 4997, P75 = 7494, P90 = 8997, P99 = 9893, P999 = 9986
  // Shuffle #2: P50 = 4995, P75 = 7498, P90 = 8994, P99 = 9892, P999 = 9985
  // Shuffle #3: P50 = 4997, P75 = 7490, P90 = 8997, P99 = 9891, P999 = 9987
  // Shuffle #4: P50 = 4994, P75 = 7494, P90 = 8992, P99 = 9893, P999 = 9986
  // Shuffle #5: P50 = 4992, P75 = 7495, P90 = 8992, P99 = 9891, P999 = 9984
  // Shuffle #6: P50 = 4994, P75 = 7492, P90 = 8990, P99 = 9893, P999 = 9984
  // Shuffle #7: P50 = 4995, P75 = 7494, P90 = 8992, P99 = 9895, P999 = 9989
  // Shuffle #8: P50 = 4997, P75 = 7496, P90 = 8993, P99 = 9891, P999 = 9989
  // Shuffle #9: P50 = 4996, P75 = 7494, P90 = 8992, P99 = 9894, P999 = 9986
  std::vector<int> sampleSizes({3, 10, 20, 100, 1000, 10000});
  for (int size : sampleSizes)
  {
    printf("Testing the behavior with {1, 2, ..., %d} with shuffling\n",size);
    std::vector<CKMS::Quantile> v({{0.5, 0.001}, {0.99, 0.001}, {1, 0}});
    std::vector<int> samples;
    auto rng = std::default_random_engine {};
    for (int i = 1; i <= size; i++)
    {
        samples.push_back(i);
    }
    for (int n = 0; n < 10; n++)
    {
        // When n = 0, we give {1, 2, ..., n}.
        // When n >= 1, the order is shuffled.
        auto ckms = CKMS(v);
        for (auto const& s : samples)
        {
            ckms.insert(s);
            ckms.get(0.5); // force ckms to clear the buffer.
        }
        printf("Shuffle #%d: P50 = %2d, P75 = %2d, P90 = %2d, P99 = %2d, P999 = %2d\n",
                n,
                int(ckms.get(0.5)),
                int(ckms.get(0.75)),
                int(ckms.get(0.9)),
                int(ckms.get(0.99)),
                int(ckms.get(0.999)));
        std::shuffle(samples.begin(), samples.end(), rng);
    }
  }
}

