#include <gtest/gtest.h>
#include <iostream>
#include "evolution.h"


TEST(genetics, mutate) {
  Agent a(4, 0x00);
  Agent b(a);

  Parameters params;
  params.muNumMutations = 2.5;

  unsigned numMutations = mutate(a, params);
  EXPECT_LE(distance(a, b), numMutations);
}

TEST(genetics, crossover) {
  Agent a(20, 0x00);
  Agent b(20, 0xFF);

  Parameters params;
  params.muNumCrossovers = 2.5;

  Agent c = crossover(a, b, params);
  const Buffer &cChromosome = c.getChromosomeConst();
  std::cout << "0b";
  for (unsigned k = 0; k < 8 * cChromosome.size(); k++) {
    std::cout << (int) cChromosome.getBit(k);
  }
  std::cout << std::endl;
}
