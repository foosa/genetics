#include <boost/program_options.hpp>
#include <iostream>
#include "parameters.h"


/* Namespace alias */
namespace po = boost::program_options;


int main(int argc, const char *argv[]) {

  Parameters p;

  try {
    po::options_description input("Inputs");
    input.add_options()
    ("help", "display help message")
    ("population-size",
     po::value<unsigned> (&(p.sizePopulation))->default_value(10000),
     "Population size at the start of each round")
    ("chromosome-size",
     po::value<unsigned> (&(p.sizeChromosome))->default_value(16),
     "Number of bytes in each chromosome")
    ("mean-num-mutations",
     po::value<double> (&(p.muNumMutations))->default_value(1.5),
     "Mean number of random bit flips per 'mutate' event")
    ("mean-num-crossovers",
     po::value<double> (&(p.muNumCrossovers))->default_value(2),
     "Mean number of crossover events between parent chromosomes")
    ("lambda-energy",
     po::value<double> (&(p.lambdaEnergy))->default_value(10),
     "Energy given to every child at birth")
    ("sigma-predation",
     po::value<double> (&(p.sigmaPredation))->default_value(0.25),
     "Stdev of normal distribution used in 'predation' events")
    ("lambda-predation",
     po::value<double> (&(p.lambdaPredation))->default_value(0.5),
     "Parameter used in 'predation' events")
    ("lambda-score-feed",
     po::value<double> (&(p.lambdaScoreFeed))->default_value(1),
     "Energy proportional to predation score during 'feed' events")
    ("lambda-entropy-feed",
     po::value<double> (&(p.lambdaEntropyFeed))->default_value(10),
     "Energy proportional to prey entropy during 'feed' events")
    ("mean-energy-starve",
     po::value<double> (&(p.muEnergyStarve))->default_value(1),
     "Mean energy required to survive a 'starve' event")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, input), vm);
    po::notify(vm);

    if (argc == 1) {
      std::cout << input << std::endl;
      return 1;
    }

    if (vm.count("help")) {
      std::cout << input << std::endl;
      return 1;
    }

    /* Remove me when we are working */
    std::cout << "p.sizePopulation = " << p.sizePopulation;
    std::cout << std::endl;

  }

  catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
