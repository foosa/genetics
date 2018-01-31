#include <gtest/gtest.h>
#include <iostream>
#include "evolution.h"


TEST(genetics, mutate) {
  Buffer a(4, 0x00);
  Buffer b(a);

  unsigned numMutations = mutate(a, 2.5);
  EXPECT_LE(distance(a, b), numMutations);
}

TEST(genetics, crossover) {
  Buffer a(20, 0x00);
  Buffer b(20, 0xFF);

  Buffer c = crossover(a, b, 1);
  std::cout << "0b";
  for (unsigned k = 0; k < 8 * c.size(); k++) {
    std::cout << (int) c.getBit(k);
  }
  std::cout << std::endl;
}
