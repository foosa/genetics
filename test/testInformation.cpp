#include <gtest/gtest.h>
#include <cmath>
#include <fstream>
#include "evolution.h"


TEST(information, buffer) {
  Buffer a(1, 0x00);
  Buffer b(1, 0xFF);

  EXPECT_EQ(a, ~b);

  Buffer c(1, 0x01);
  Buffer d = b ^ c;
  Buffer e(1, 0xFE);
  EXPECT_EQ(d, e);

  Buffer f = d ^ c;
  EXPECT_EQ(f, b);
  EXPECT_EQ(b ^ b, a);
}

TEST(information, hammingSingleChar) {
  Buffer buffer(1, 0x00);
  EXPECT_EQ(hammingWeight(buffer), 0);

  buffer[0] = 0x01;
  EXPECT_EQ(hammingWeight(buffer), 1);

  buffer[0] = 0x02;
  EXPECT_EQ(hammingWeight(buffer), 1);

  buffer[0] = 0x03;
  EXPECT_EQ(hammingWeight(buffer), 2);

  buffer[0] = 0x10;
  EXPECT_EQ(hammingWeight(buffer), 1);

  buffer[0] = 0xFF;
  EXPECT_EQ(hammingWeight(buffer), 8);
}


TEST(information, hammingString) {
  Buffer buffer(3, 0x00);
  buffer[0] = 0x03; // +2
  buffer[1] = 0x01; // +1
  buffer[2] = 0xF3; // +4 + 2

  EXPECT_EQ(hammingWeight(buffer), 2 + 1 + 4 + 2);
}


TEST(information, shannonEntropy) {
  Buffer buffer1(4, 0x00);
  EXPECT_EQ(shannonEntropy(buffer1), 0);

  Buffer buffer2(4, 0xFF);
  EXPECT_EQ(shannonEntropy(buffer2), 0);

  Buffer buffer3(4, 0x00);
  buffer3[0] = 0xFF;
  buffer3[2] = 0xFF;
  EXPECT_EQ(shannonEntropy(buffer3), - log2(0.5));
}


TEST(information, distance) {
  Buffer a = "test";
  EXPECT_EQ(distance(a, a), 0);

  Buffer b = "bar ";
  Buffer c = "star";
  EXPECT_NE(distance(a, b), 0);

  EXPECT_EQ(distance(a, b), distance(b, a));
  unsigned dAB = distance(a, b);
  unsigned dBC = distance(b, c);
  unsigned dAC = distance(a, c);
  EXPECT_LE(dAC, dAB + dBC);
}

TEST(information, bitFlip) {
  Buffer a(20, 0x00);
  Buffer b(a);
  Buffer c(a.size(), 0xFF);

  EXPECT_EQ(distance(a, b), 0);
  for (unsigned k = 0; k < 8 * a.size(); k++) {
    b.flipBit(k);
    EXPECT_EQ(distance(a, b), 1 + k);
  }

  EXPECT_EQ(b, c);
}

TEST(information, serialization) {
  Buffer a(20, 0x00);
  a[0] = 0x01;
  a[1] = 0x02;
  a[2] = 0x03;

  {
    /* Scope */
    std::ofstream ofs("buffer.bin");
    boost::archive::binary_oarchive oa(ofs);

    /* Serialize the buffer */
    oa << a;
  }

  {
    /* Restore the buffer */
    std::ifstream ifs("buffer.bin");
    boost::archive::binary_iarchive ia(ifs);

    Buffer b;
    ia >> b;

    EXPECT_EQ(a.size(), b.size());
    for (unsigned k = 0; k < a.size(); k++) {
      EXPECT_EQ(a[k], b[k]);
    }
  }
}

