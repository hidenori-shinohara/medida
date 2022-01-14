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


TEST(CKMSTest, aCKMSPercentileSmallSamples) {
  // Input size =     1 (got, exp) => P50 =     1 vs     1, P75 =     1 vs     1, P90 =     1 vs     1, P99 =     1 vs     1, P999 =     1 vs     1
  // Input size =     2 (got, exp) => P50 =     1 vs     1, P75 =     1 vs     2, P90 =     1 vs     2, P99 =     1 vs     2, P999 =     1 vs     2
  // Input size =     3 (got, exp) => P50 =     1 vs     2, P75 =     2 vs     3, P90 =     2 vs     3, P99 =     2 vs     3, P999 =     2 vs     3
  // Input size =     4 (got, exp) => P50 =     2 vs     2, P75 =     2 vs     3, P90 =     2 vs     4, P99 =     2 vs     4, P999 =     2 vs     4
  // Input size =     5 (got, exp) => P50 =     2 vs     3, P75 =     2 vs     4, P90 =     3 vs     5, P99 =     3 vs     5, P999 =     3 vs     5
  // Input size =    10 (got, exp) => P50 =     4 vs     5, P75 =     6 vs     8, P90 =     8 vs     9, P99 =     8 vs    10, P999 =     8 vs    10
  // Input size =    15 (got, exp) => P50 =     6 vs     8, P75 =    10 vs    12, P90 =    12 vs    14, P99 =    13 vs    15, P999 =    13 vs    15
  // Input size =    20 (got, exp) => P50 =     9 vs    10, P75 =    14 vs    15, P90 =    17 vs    18, P99 =    18 vs    20, P999 =    18 vs    20
  // Input size =    30 (got, exp) => P50 =    14 vs    15, P75 =    21 vs    23, P90 =    26 vs    27, P99 =    28 vs    30, P999 =    28 vs    30
  // Input size =    40 (got, exp) => P50 =    19 vs    20, P75 =    29 vs    30, P90 =    35 vs    36, P99 =    38 vs    40, P999 =    38 vs    40
  // Input size =    50 (got, exp) => P50 =    24 vs    25, P75 =    36 vs    38, P90 =    44 vs    45, P99 =    48 vs    50, P999 =    48 vs    50
  // Input size =   100 (got, exp) => P50 =    49 vs    50, P75 =    74 vs    75, P90 =    89 vs    90, P99 =    98 vs    99, P999 =    98 vs   100
  // Input size =   110 (got, exp) => P50 =    54 vs    55, P75 =    81 vs    83, P90 =    98 vs    99, P99 =   107 vs   109, P999 =   108 vs   110
  // Input size =   120 (got, exp) => P50 =    59 vs    60, P75 =    89 vs    90, P90 =   107 vs   108, P99 =   117 vs   119, P999 =   118 vs   120
  // Input size =   130 (got, exp) => P50 =    64 vs    65, P75 =    96 vs    98, P90 =   116 vs   117, P99 =   127 vs   129, P999 =   128 vs   130
  // Input size =   140 (got, exp) => P50 =    69 vs    70, P75 =   104 vs   105, P90 =   125 vs   126, P99 =   137 vs   139, P999 =   138 vs   140
  // Input size =   150 (got, exp) => P50 =    74 vs    75, P75 =   111 vs   113, P90 =   134 vs   135, P99 =   147 vs   149, P999 =   148 vs   150
  // Input size =   200 (got, exp) => P50 =    99 vs   100, P75 =   149 vs   150, P90 =   179 vs   180, P99 =   197 vs   198, P999 =   198 vs   200
  // Input size =   250 (got, exp) => P50 =   124 vs   125, P75 =   186 vs   188, P90 =   224 vs   225, P99 =   246 vs   248, P999 =   248 vs   250
  // Input size =   300 (got, exp) => P50 =   149 vs   150, P75 =   224 vs   225, P90 =   269 vs   270, P99 =   296 vs   297, P999 =   298 vs   300
  // Input size =   400 (got, exp) => P50 =   199 vs   200, P75 =   299 vs   300, P90 =   359 vs   360, P99 =   395 vs   396, P999 =   398 vs   400
  // Input size =   500 (got, exp) => P50 =   249 vs   250, P75 =   374 vs   375, P90 =   449 vs   450, P99 =   494 vs   495, P999 =   498 vs   500
  // Input size =  1000 (got, exp) => P50 =   500 vs   500, P75 =   750 vs   750, P90 =   898 vs   900, P99 =   989 vs   990, P999 =   998 vs   999
  // Input size =  5000 (got, exp) => P50 =  2499 vs  2500, P75 =  3749 vs  3750, P90 =  4496 vs  4500, P99 =  4948 vs  4950, P999 =  4994 vs  4995
  // Input size = 10000 (got, exp) => P50 =  4996 vs  5000, P75 =  7496 vs  7500, P90 =  8995 vs  9000, P99 =  9898 vs  9900, P999 =  9988 vs  9990
  std::vector<int> sampleSizes({1, 2, 3, 4, 5, 10, 15, 20, 30, 40, 50, 100, 110, 120, 130, 140, 150, 200, 250, 300, 400, 500, 1000, 5000, 10000});
  for (int size : sampleSizes)
  {
    std::vector<CKMS::Quantile> v({{0.5, 0.001}, {0.99, 0.001}, {1, 0}});
    auto ckms = CKMS(v);
    for (int s = 1; s <= size; s++)
    {
        ckms.insert(s);
    }
    printf("Input size = %5d (got, exp) => P50 = %5d vs %5d, P75 = %5d vs %5d, P90 = %5d vs %5d, P99 = %5d vs %5d, P999 = %5d vs %5d\n",
            size,
            int(ckms.get(0.5)), int(ceil(0.5 * size)),
            int(ckms.get(0.75)), int(ceil(0.75 * size)),
            int(ckms.get(0.9)), int(ceil(0.9 * size)),
            int(ckms.get(0.99)), int(ceil(0.99 * size)),
            int(ckms.get(0.999)), int(ceil(0.999 * size)));
  }
}

