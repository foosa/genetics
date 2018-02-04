#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include "agent.h"


TEST(genetics, mutate) {
  Agent a(4, 0x00);
  Agent b(a);

  Parameters params;
  params.muNumMutations = 2.5;

  unsigned numMutations = mutate(a, params);
  const Buffer &ca = a.getChromosomeConst();
  const Buffer &cb = b.getChromosomeConst();
  EXPECT_LE(distance(ca, cb), numMutations);
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

TEST(genetics, serialization) {
  Agent a(4, 0x00);
  a[1] = 0x01;
  a[2] = 0x02;
  a[3] = 0x03;

  a.setEnergy(3);

  {
    std::ofstream ofs("agent.bin");
    boost::archive::binary_oarchive oa(ofs);

    /* Serialize the agent */
    oa << a;
  }

  {
    /* Restore the agent */
    std::ifstream ifs("agent.bin");
    boost::archive::binary_iarchive ia(ifs);

    Agent b;
    ia >> b;

    const Buffer &ca = a.getChromosomeConst();
    const Buffer &cb = b.getChromosomeConst();

    EXPECT_EQ(ca.size(), cb.size());
    for (unsigned k = 0; k < ca.size(); k++) {
      EXPECT_EQ(ca[k], cb[k]);
    }

    EXPECT_EQ(a.getEnergy(), b.getEnergy());
  }
}
