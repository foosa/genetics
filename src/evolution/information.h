#ifndef INFORMATION_H
#define INFORMATION_H

#include <vector>


/* -------------------------------------------------------------------------- *
 * Information theory                                                         *
 * -------------------------------------------------------------------------- */

class Buffer : public std::vector<char> {

 public:

  /* Helper constructors */
  Buffer();
  Buffer(const char *c_str);
  Buffer(const char *c_str, unsigned size);
  Buffer(unsigned num, char c);

  /* Bitwise arithmetic operations */
  Buffer &operator^= (const Buffer &obj);
  const Buffer operator^ (const Buffer &obj) const;
  const Buffer operator~() const;

  /* Get the value of the `i`th bit */
  u_int8_t getBit(unsigned index) const;

  /* Flip the `i`th bit */
  void flipBit(unsigned index);
};


/**
 * @brief Calculate the Hamming weight of a byte string, interpreting `buffer`
 *  as a string of bytes.
 *
 * @param buffer
 *
 * @return Hamming weight (number of ones)
 */
unsigned hammingWeight(const Buffer &buffer);

/**
 * @brief Calculate the Shannon entropy of a byte string
 *
 * @param buffer
 *
 * @return Shannon entropy
 */
double shannonEntropy(const Buffer &buffer);

/**
 * @brief Calculate the distance between two byte strings, defined as the
 *  metric `d(a, b) = H(a - b)`, where `H(x)` is the Hamming weight.  The
 *  strings must be the same size.
 *
 * @note This function throws an exception when byte strings are unequally
 *  sized.
 *
 * @param a first byte string
\ * @param b second byte string
 *
 * @return distance
 */
unsigned distance(const Buffer &a, const Buffer &b);


#endif /* end of include guard: INFORMATION_H */
