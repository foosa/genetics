#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <boost/serialization/access.hpp>


/**
 * @brief A simple class to store the model parameters.  Many of the models
 *  functions accept a reference to a `Parameters` object.  If you find you
 *  need additional parameters for a new function implementation, you should
 *  subclass this class.
 */
typedef struct {

 private:
  friend class boost::serialization::access;

  /* Serialization */
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar &sizePopulation;
    ar &sizeChromosome;
    ar &muNumMutations;
    ar &muNumCrossovers;
    ar &lambdaEnergy;
    ar &sigmaPredation;
    ar &lambdaPredation;
    ar &lambdaScoreFeed;
    ar &lambdaEntropyFeed;
    ar &muEnergyStarve;
    ar &muMating;
  }

 public:

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
  double muMating;          //! Average `selectivity` for mating.  A lower
  //  number corresponds to higher selection.  Must be in [0, 1].

} Parameters;


#endif /* end of include guard: PARAMETERS_H */
