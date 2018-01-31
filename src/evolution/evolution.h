#ifndef EVOLUTION_H
#define EVOLUTION_H

#include "information.h"



/* -------------------------------------------------------------------------- *
 * Genetics                                                                   *
 * -------------------------------------------------------------------------- */


/**
 * @brief A simple class to store the model parameters.  Many of the models
 *  functions accept a reference to a `Parameters` object.  If you find you
 *  need additional parameters for a new function implementation, you should
 *  subclass this class.
 */
typedef struct {

  unsigned sizePopulation;  //! Maximum number of agents in the population
  unsigned sizeChromosome;  //! Number of bytes in each agent's chromosome

  /* Selection parameters */
  double muNumMutations;    //! Mean number of mutations per `mutate`
  double muNumCrossovers;   //! Mean number of crossovers per `crossover`
  double lambdaEnergy;      //! Energy given to every child at birth.
  double sigmaPredation;    //! Standard deviation of the normal dist. used
  //  for `predation` operations
  double lambdaPredation;   //! Parameter used for `predation` operations
  double lambdaScoreFeed;   //! Parameter used for `feed` operations
  double lambdaEntropyFeed; //! Parameter used for `feed` operations
  double muEnergyStarve;    //! Mean energy required to survive a `starve`

} Parameters;


/**
 * @brief A class representing a single organism
 */
class Agent {
 private:
  Buffer m_chromosome;  //! Chromosome
  unsigned m_size;      //! Size (in number of bytes) of the chromosome
  double m_energy;      //! Energy of the agent.  Used for the `feed` and
  //  `starve` operations

 public:

  /**
   * @brief Allocates a new agent
   *
   * @param size size of the agent's chromosome in bytes
   * @param c character to fill chromosome with
   * @param energy agent's energy
   */
  Agent(unsigned size = 128, char c = 0x00, double energy = 0);

  /**
   * @brief Allocates a new agent from a chromosome
   *
   * @param chromosome chromosome to use
   * @param energy agent's energy
   */
  Agent(const Buffer &chromosome, double energy = 0);

  /* Copy constructor */
  Agent(const Agent &agent);

  /* Array-like access to the chromosome bytes */
  char &operator[](unsigned index);
  const char &operator[](unsigned index) const;

  /* Getter for the chromosome */
  Buffer &getChromosome(void);
  const Buffer &getChromosomeConst(void) const;
};


/**
 * @brief Performs `n` mutations (bit flips) to the chromosome.  `n` is a
 *  random integer drawn from a Poisson distribution with mean value
 *  `muNumMutations`.  The mutation locations are drawn from a uniform
 *  distribution (any particular bit is just as likely to flip as another).
 *  This means that if more than one bit flip occurs, there is a chance that
 *  the next flip inverts a previous flip, resulting in no net change to
 *  the chromosome.
 *
 * @param agent
 * @param params
 *
 * @return numMutations (`n`)
 */
unsigned mutate(Agent &agent, const Parameters &params);

/**
 * @brief Performs `n` crossovers between two parent chromosomes to create a
 *  child chromosome.  `n` is a random integer drawn from a Poisson
 *  distribution with mean value `muNumCrossovers`.  The crossover locations
 *  are drawn from a uniform distribution.
 *
 * @param father
 * @param mother
 * @param params
 *
 * @return child agent
 */
Agent crossover(const Agent &father, const Agent &mother,
                const Parameters &params);

typedef enum {
  PREDATION_BOTH_SURVIVE,
  PREDATION_FIRST_SURVIVES,
  PREDATION_SECOND_SURVIVES
} PredationOutcome;

/**
 * @brief Performs a `predation` operation between two individuals.  There are
 *  three possible return values ...
 *
 *    Result    Interpretation
 *
 *    0     Both agents survive encounter but neither gets to eat
 *    1     The first agent kills the second agent and gets to eat
 *    2     The second agent kills the first agent and gets to eat
 *
 *  In order to naturally produce sub-populations which emulate ecological
 *  `niches` we are using a cost function that encourages specialization
 *  (i.e. predator agents that are optimally tuned to kill prey agents).
 *  We do not assign individuals `predator` and `prey` roles; our goal here
 *  is to simulate an encounter and allow both agents an opportunity to
 *  eat the other.
 *
 * @note There are two parameters that control predation pressure:
 *  (1) `sigmaPredation` and (2) `lambdaPredation` (hereafter called `s` and
 *  `l`).  Increasing `s` increases the probability that the disadvantaged
 *  individual will survive or win the encounter.  Increasing `l` increases the
 *  `escape rate` for the prey.
 *
 * @note Let `p` be a random variable drawn from the normal distribution
 *  `N(0, S^2)` where `S = sqrt(n) * s` and `n` is the number of bits in the
 *  chromosomes.  If `p < K(a, b) and L < K(a, b)`, where `L = sqrt(n) * l`
 *  and `K(a, b)` is the score function then individual `a` is declared the
 *  winner.  Else if `p < - K(a, b)` and `L < - K(a, b)` then individual `b`
 *  is the winner.  Otherwise both individuals survive the encounter.  The
 *  score is computed as follows ...
 *
 *      K(a, b) = \sum_{i=0}^{n/2} F(a_{2i}, b_{2i})
 *
 *  where `a_{2i}` and `b_{2i}` are two-bit strings starting at the chromosome
 *  index location `2i` and `F(.,.)` is a look-up table that encodes a rock-
 *  paper-scissors like game that increases or deducts points from the
 *  chromosome.  The resolution table for the scoring system is represented in
 *  the graph diagram below ...
 *
 *      "00" <------- "11"
 *        |            ^
 *        |            |
 *        V            |
 *      "01" -------> "10"
 *
 *  The score function is anti-symmetric.  For any two bit strings `a` and `b`
 *  `K(a, b) = - K(b, a)`.
 *
 * @note if `a` and `b` are both random bit strings then the score `K(a, b)` is
 *  a stochastic random variable undergoing Brownian motion with a variance
 *  that grows as `O(n)`, where `n` is the number of bits.  We scale S and L
 *  with `sqrt(n)` so that the parameters `s` and `l` apply the same average
 *  selection pressure regardless of bit string length.
 *
 * @param a chromosome of individual `a`
 * @param b chromosome of individual `b`
 *
 * @return reference to the winning chromosome
 */
PredationOutcome predation(const Agent &a, const Agent &b,
                           const Parameters &params);

void feed(Agent &predator, Agent &prey, const Parameters &params);

int starve(const Agent &agent, const Parameters &params);

unsigned hammingWeight(const Agent &a);

double shannonEntropy(const Agent &a);

unsigned distance(const Agent &a, const Agent &b);

#endif /* end of include guard: EVOLUTION_H */
