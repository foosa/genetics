#ifndef ECOSYSTEM_H
#define ECOSYSTEM_H

// #include <mutex>
#include <memory>
#include "evolution.h"



/* -------------------------------------------------------------------------- *
 * Ecosystem                                                                  *
 * -------------------------------------------------------------------------- */


class Ecosystem {
 private:

  std::vector<std::unique_ptr<Agent>> m_agents;
  Parameters m_parameters;

  friend class boost::serialization::access;

  /* Serialization */
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar &m_parameters;
    ar &m_agents;
  }

  void runOnceSerial(void);
  void runOnceThread(unsigned numThreads);

 public:

  Ecosystem(const Parameters &params);

  void run(unsigned numIterations = 1000);

  /* Simple statistics and diagnostics */
  double meanEntropy(void);
  double stdevEntropy(void);
  double meanSurvivalFraction(void);
};


#endif /* end of include guard: ECOSYSTEM_H */
