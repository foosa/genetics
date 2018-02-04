#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include "ecosystem.h"

TEST(ecosystem, serialization) {
  Parameters params;
  params.sizePopulation = 100;
  params.sizeChromosome = 128;

  Ecosystem e(params);

  {
    std::ofstream ofs("ecosystem.bin");
    boost::archive::binary_oarchive oa(ofs);

    /* Serialize the ecosystem */
    oa << e;
  }

  {
    /* Restore the agent */
    std::ifstream ifs("ecosystem.bin");
    boost::archive::binary_iarchive ia(ifs);

    Ecosystem e2;
    ia >> e2;
  }
}
