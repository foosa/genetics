#include <cmath>
#include <cassert>
#include <time.h>
#include <algorithm>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "agent.h"


/* -------------------------------------------------------------------------- *
 * Genetics                                                                   *
 * -------------------------------------------------------------------------- */

Agent::Agent(unsigned size, char c, double energy) {
  m_chromosome = Buffer(size, c);
  m_energy = energy;
}

Agent::Agent(const Buffer &chromosome, double energy) {
  m_chromosome = chromosome;
  m_energy = energy;
}

Agent::Agent(const Agent &agent) {
  m_chromosome = agent.m_chromosome;
  m_energy = agent.m_energy;
}

Buffer &Agent::getChromosome(void) {
  return m_chromosome;
}

const Buffer &Agent::getChromosomeConst(void) const {
  return m_chromosome;
}

double Agent::getEnergy(void) const {
  return m_energy;
}

void Agent::setEnergy(double energy) {
  m_energy = energy;
}

char &Agent::operator[](unsigned index) {
  return m_chromosome[index];
}

const char &Agent::operator[](unsigned index) const {
  return m_chromosome[index];
}

unsigned mutate(Agent &agent, const Parameters &params) {
  const gsl_rng_type *T = gsl_rng_default;
  gsl_rng *rng = gsl_rng_alloc(T);
  gsl_rng_set(rng, time(NULL));

  Buffer &chromosome = agent.getChromosome();
  unsigned size = chromosome.size();
  unsigned numMutations = gsl_ran_poisson(rng, params.muNumMutations);

  for (unsigned k = 0; k < numMutations; k++) {
    unsigned index = gsl_rng_uniform_int(rng, size);
    chromosome.flipBit(index);
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

const Buffer &CrossoverMediator::cross(void) {
  const Buffer *active = NULL;
  m_active = (m_active + 1) % 2;
  active = m_buffers[m_active];
  return *active;
}

Agent crossover(const Agent &father, const Agent &mother,
                const Parameters &params) {
  const gsl_rng_type *T = gsl_rng_default;
  gsl_rng *rng = gsl_rng_alloc(T);
  gsl_rng_set(rng, time(NULL));

  /* Get parent chromosomes */
  const Buffer &a = father.getChromosomeConst();
  const Buffer &b = mother.getChromosomeConst();
  unsigned size = a.size();

  /* Get sorted list of cross indices */
  unsigned numCross = gsl_ran_poisson(rng, params.muNumCrossovers);
  std::vector<unsigned> indices;
  unsigned index, nextIndex;
  unsigned j, k;

  for (k = 0; k < numCross; k++) {
    index = gsl_rng_uniform_int(rng, size);
    indices.push_back(index);
  }
  std::sort(indices.begin(), indices.end());

  /* Apply crosses */
  CrossoverMediator mediator(a, b, gsl_rng_uniform_int(rng, 2));
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
u_int8_t getTwoBits(const Buffer &buffer, unsigned index) {
  unsigned offset =  index / 4;
  unsigned k = index % 4;

  /* Get the bits */
  char b = buffer[offset];
  u_int8_t bits = ((b >> k) & 0x03);
  return bits;
}

PredationOutcome predation(const Agent &first, const Agent &second,
                           const Parameters &params) {

  const Buffer &a = first.getChromosomeConst();
  const Buffer &b = second.getChromosomeConst();

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
    return PREDATION_FIRST_SURVIVES;
  }

  else if ((p < - score) && (L < - score)) {
    return PREDATION_SECOND_SURVIVES;
  }

  else {
    return PREDATION_BOTH_SURVIVE;
  }
}

void feed(Agent &predator, Agent &prey, const Parameters &params) {
  const Buffer &c = prey.getChromosomeConst();
  double entropy = shannonEntropy(c);
  double energy = predator.getEnergy();

  energy += params.lambdaScoreFeed * entropy + 1.0;
  predator.setEnergy(energy);
}

int starve(Agent &agent, const Parameters &params) {
  /* Draw a random number */
  const gsl_rng_type *T = gsl_rng_default;
  gsl_rng *rng = gsl_rng_alloc(T);
  gsl_rng_set(rng, time(NULL));

  unsigned p = gsl_ran_poisson(rng, params.muEnergyStarve);
  gsl_rng_free(rng);

  /* Check outcome */
  double e = agent.getEnergy();
  if (p < e) {
    agent.setEnergy(e - p);
    return 0;
  }

  else {
    agent.setEnergy(0);
    return 1;
  }
}

int mate(const Agent &a, const Agent &b, const Parameters &params) {
  const Buffer &ca = a.getChromosomeConst();
  const Buffer &cb = b.getChromosomeConst();

  /* Hamming distance divided by chromosome size */
  double d = distance(ca, cb);
  d = d / ca.size();

  /* Choose a random Beta-distributed number.  We are using a beta-distribution
   * where the mode is pinned to be exactly zero, e.g.
   *
   *    0 = (alpha - 1) / (alpha + beta - 2)
   *
   * so alpha = 1.  The mean is mu = 1 / (1 + beta)
   */
  double mu = params.muMating;
  double beta = (1.0 / mu) - 1.0;

  const gsl_rng_type *T = gsl_rng_default;
  gsl_rng *rng = gsl_rng_alloc(T);
  gsl_rng_set(rng, time(NULL));
  double p = gsl_ran_beta(rng, 1, beta);
  gsl_rng_free(rng);

  if (d < p) {
    return 1;
  }

  else {
    return 0;
  }
}

