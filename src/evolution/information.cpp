#include <cmath>
#include <cstring>
#include "information.h"


/* -------------------------------------------------------------------------- *
 * Information theory and bitstring buffers                                   *
 * -------------------------------------------------------------------------- */

Buffer::Buffer() { }

Buffer::Buffer(const char *c_str) {
  for (unsigned k = 0; k < strlen(c_str); k++) {
    this->push_back(c_str[k]);
  }
}

Buffer::Buffer(const char *c_str, unsigned size) {
  for (unsigned k = 0; k < size; k++) {
    this->push_back(c_str[k]);
  }
}

Buffer::Buffer(unsigned num, char c) {
  for (unsigned k = 0; k < num; k++) {
    this->push_back(c);
  }
}

Buffer &Buffer::operator^= (const Buffer &obj) {
  Buffer &self = *this;
  for (unsigned k = 0; k < size(); k++) {
    self[k] = self[k] ^ obj[k];
  }
  return self;
}

const Buffer Buffer::operator^ (const Buffer &obj) const {
  Buffer copy(*this);
  copy ^= obj;
  return copy;
}

const Buffer Buffer::operator~() const {
  const Buffer &self = *this;
  Buffer copy(*this);
  for (unsigned k = 0; k < size(); k++) {
    copy[k] = ~self[k];
  }
  return copy;
}

static const char flipMask[] = "\x01\x02\x04\x08\x10\x20\x40\x80";

void Buffer::flipBit(unsigned index) {
  Buffer &self = *this;
  unsigned offset = index / 8;
  unsigned k = index % 8;

  /* Flip the bit */
  char mask = flipMask[k];
  self[offset] = self[offset] ^ mask;
}

u_int8_t Buffer::getBit(unsigned index) const {
  const Buffer &self = *this;
  unsigned offset =  index / 8;
  unsigned k = index % 8;

  /* Get the bit */
  char b = self[offset];
  u_int8_t bit = ((b >> k) & 0x01);
  return bit;
}

/* Used in hammingWeight */
static const unsigned hammingNumOnes[] = {
  0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
};

/**
 * @brief Returns the number of binary ones in an unsigned char
 *
 * @param x
 */
static unsigned hammingCountOnes(unsigned char x) {
  unsigned numOnes;
  numOnes = hammingNumOnes[x & 0x0f];
  numOnes += hammingNumOnes[x >> 4];
  return numOnes;
}

unsigned hammingWeight(const Buffer &buffer) {
  unsigned weight = 0;
  for (unsigned k = 0; k < buffer.size(); k++) {
    weight += hammingCountOnes(buffer[k]);
  }
  return weight;
}

double shannonEntropy(const Buffer &buffer) {
  unsigned numBits = buffer.size() * 8;
  unsigned numOnes = hammingWeight(buffer);
  unsigned numZeros = numBits - numOnes;

  /* Probabilities */
  double p0 = ((double) numZeros) / numBits;
  double p1 = ((double) numOnes) / numBits;

  double S;
  if (numZeros == 0) {
    S = - p1 * log2(p1);
  } else if (numOnes == 0) {
    S = - p0 * log2(p0);
  } else {
    S = - (p0 * log2(p0) + p1 * log2(p1));
  }

  return S;
}

unsigned distance(const Buffer &a, const Buffer &b) {
  return hammingWeight(a ^ b);
}


