#include <cmath>
#include <cstring>
#include <time.h>
#include <algorithm>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "evolution.h"


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

Buffer &
Buffer::operator^= (const Buffer &obj) {
  Buffer &self = *this;
  for (unsigned k = 0; k < size(); k++) {
    self[k] = self[k] ^ obj[k];
  }
  return self;
}

const Buffer
Buffer::operator^ (const Buffer &obj) const {
  Buffer copy(*this);
  copy ^= obj;
  return copy;
}

const Buffer
Buffer::operator~() const {
  const Buffer &self = *this;
  Buffer copy(*this);
  for (unsigned k = 0; k < size(); k++) {
    copy[k] = ~self[k];
  }
  return copy;
}

static const char flipMask[] = "\x01\x02\x04\x08\x10\x20\x40\x80";

void
Buffer::flipBit(unsigned index) {
  Buffer &self = *this;
  unsigned offset = index / 8;
  unsigned k = index % 8;

  /* Flip the bit */
  char mask = flipMask[k];
  self[offset] = self[offset] ^ mask;
}

u_int8_t
Buffer::getBit(unsigned index) {
  Buffer &self = *this;
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
static unsigned
hammingCountOnes(unsigned char x) {
  unsigned numOnes;
  numOnes = hammingNumOnes[x & 0x0f];
  numOnes += hammingNumOnes[x >> 4];
  return numOnes;
}

unsigned
hammingWeight(const Buffer &buffer) {
  unsigned weight = 0;
  for (unsigned k = 0; k < buffer.size(); k++) {
    weight += hammingCountOnes(buffer[k]);
  }
  return weight;
}

double
shannonEntropy(const Buffer &buffer) {
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

unsigned
distance(const Buffer &a, const Buffer &b) {
  return hammingWeight(a ^ b);
}


/* -------------------------------------------------------------------------- *
 * Genetics                                                                   *
 * -------------------------------------------------------------------------- */

Agent::Agent(unsigned size, char c, double energy) {
  m_chromosome = Buffer(size, c);
  m_size = size;
  m_energy = energy;
}

Agent::Agent(const Buffer &chromosome, double energy) {
  m_chromosome = chromosome;
  m_size = m_chromosome.size();
  m_energy = energy;
}

char &
Agent::operator[](unsigned index) {
  return m_chromosome[index];
}

const char &
Agent::operator[](unsigned index) const {
  return m_chromosome[index];
}

unsigned
mutate(Agent &agent, const Parameters &params) {
  const gsl_rng_type *T = gsl_rng_default;
  gsl_rng *rng = gsl_rng_alloc(T);
  gsl_rng_set(rng, time(NULL));

  unsigned numMutations = gsl_ran_poisson(rng, params.muNumMutations);
  for (unsigned k = 0; k < numMutations; k++) {
    unsigned index = gsl_rng_uniform_int(rng, agent.m_size);
    agent.m_chromosome.flipBit(index);
  }

  gsl_rng_free(rng);
  return numMutations;
}

/**
 * @brief Helper class to switch (toggle) between parent chromosomes
 */
class CrossoverMediator {
 private:
  std::vector<const Buffer *> m_buffers;
  unsigned m_active;

 public:
  CrossoverMediator(const Buffer &a, const Buffer &b, unsigned active);
  const Buffer &cross(void);
};

CrossoverMediator::CrossoverMediator(const Buffer &a, const Buffer &b,
                                     unsigned active) {
  m_buffers.push_back(&a);
  m_buffers.push_back(&b);
  m_active = active;
}

const Buffer &
CrossoverMediator::cross(void) {
  const Buffer *active = NULL;
  m_active = (m_active + 1) % 2;
  active = m_buffers[m_active];
  return *active;
}

Agent
crossover(const Agent &father, const Agent &mother,
          const Parameters &params) {
  const gsl_rng_type *T = gsl_rng_default;
  gsl_rng *rng = gsl_rng_alloc(T);
  gsl_rng_set(rng, time(NULL));

  /* Get sorted list of cross indices */
  unsigned numCross = gsl_ran_poisson(rng, params.muNumCrossovers);
  std::vector<unsigned> indices;
  unsigned index, nextIndex;
  unsigned j, k;

  for (k = 0; k < numCross; k++) {
    index = gsl_rng_uniform_int(rng, a.size());
    indices.push_back(index);
  }
  std::sort(indices.begin(), indices.end());

  /* Apply crosses */
  CrossoverMediator mediator(father.m_chromosome, mother.m_chromosome,
                             gsl_rng_uniform_int(rng, 2));
  const Buffer &active = mediator.cross();
  Buffer chromosomeChild(active);

  for (k = 0; k < numCross; k++) {
    index = indices[k];
    if (k == numCross - 1) {
      nextIndex = chromosomeChild.size();
    }

    else {
      nextIndex = indices[k + 1];
    }

    const Buffer &active = mediator.cross();
    for (j = index; j < nextIndex; j++) {
      chromosomeChild[j] = active[j];
    }
  }

  gsl_rng_free(rng);
  Agent child(chromosomeChild, params.lambdaEnergy);
  return child;
}

/* Rock paper sissors game ... */
static const int selectionScores[4][4] = {
  { 0, 1, 0, -1},
  {-1, 0, 1, 0},
  { 0, -1, 0, 1},
  { 1, 0, -1, 0}
};

/* Used to look up indices in the selection score table */
u_int8_t
getTwoBits(const Buffer &buffer, unsigned index) {
  unsigned offset =  index / 4;
  unsigned k = index % 4;

  /* Get the bits */
  char b = buffer[offset];
  u_int8_t bits = ((b >> k) & 0x03);
  return bits;
}

const Buffer &
select(const Buffer &a, const Buffer &b, double sigma) {
  const Buffer *winner;
  const Buffer *loser;
  int winnerScore;

  /* Use more general form */
  select(a, b, &winner, &loser, &winnerScore, sigma);
  return *winner;
}


int
predation(const Agent &first, const Agent &second,
          const Paramsters &params) {

  const Buffer &a = first.m_chromosome;
  const Buffer &b = second.m_chromosome;

  /* Read buffers in two-bit chunks and compute score */
  int score = 0;
  for (unsigned k = 0; k < a.size() * 4; k++) {
    u_int8_t aBits = getTwoBits(a, k);
    u_int8_t bBits = getTwoBits(b, k);
    score += selectionScores[aBits][bBits];
  }

  /* Draw a random number */
  const gsl_rng_type *T = gsl_rng_default;
  gsl_rng *rng = gsl_rng_alloc(T);
  gsl_rng_set(rng, time(NULL));

  double S = params.sigmaPredation * sqrt(a.size() * 4);
  double L = params.lambdaPredation * sqrt(a.size() * 4);
  double p = gsl_ran_gaussian(rng, S);

  gsl_rng_free(rng);

  /* Assign outcome */
  if ((p < score) && (L < score)) {
    return 1;
  }

  else if ((p < - score) && (L < - score)) {
    return 2;
  }

  else {
    return 0;
  }
}

double
feed(const Buffer &winner, const Buffer &loser, int winnerScore,
     double entropyReward) {


}


unsigned
select(const Buffer &a, const Buffer &b, double &reward,
       double entropyReward) {
  int aScore = 0;
  int bScore = 0;

  /* Read buffers in two-bit chunks and compute score */
  for (unsigned k = 0; k < a.size() * 4; k++) {
    u_int8_t aBits = getTwoBits(a, k);
    u_int8_t bBits = getTwoBits(b, k);
    aScore += selectionScores[aBits][bBits];
    bScore += selectionScores[bBits][aBits];
  }

  /* I am willing to bet that aScore = -bScore */
  assert(aScore == -bScore);
  reward = fabs(aScore) / (a.size() * 4)

  if (aScore > 0) {
    reward += entropyReward * shannonEntropy(b);
    return 0;
  }

  else {
    reward += entropyReward * shannonEntropy(a);
    return 1;
  }
}
