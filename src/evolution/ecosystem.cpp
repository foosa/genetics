#include <cassert>
#include <algorithm>
#include <random>
#include "ecosystem.h"


/* -------------------------------------------------------------------------- *
 * Helper and thread functions                                                *
 * -------------------------------------------------------------------------- */

/* Simplifies later declarations */
typedef std::vector<std::unique_ptr<Agent>> AgentVector;


/**
 * @brief Iterates through the vector of agents and sorts the agents based on
 *  whether the agent is still alive or dead.  Dead agents are pushed to the
 *  end of the vector.  Then the dead agents are erased from the vector.
 *
 * @note I haven't timed it myself, but I expect this is much faster than
 *  repeatedly calling `vector::erase`, since this will result in some elements
 *  being repeatedly shuffled, but in this function each item is moved at most
 *  one times.  The complexity of this function is O(n).
 *
 * @param agents
 *
 * @return number of dead agents
 */
unsigned removeDeadAgents(AgentVector &agents) {
  unsigned numDead = 0;
  AgentVector::iterator fIt = agents.begin();
  AgentVector::iterator bIt = agents.end() - 1;

  while (bIt > fIt) {

    /* Find first dead agent */
    while ((**fIt).getEnergy() > 0 && bIt > fIt) {
      fIt++;
    }

    /* Find last alive agent */
    while ((**bIt).getEnergy() <= 0 && bIt > fIt) {
      bIt--;
    }

    /* Swap */
    std::unique_ptr<Agent> temp = std::move(*fIt);
    *fIt = std::move(*bIt);
    *bIt = std::move(temp);
    numDead += 1;

    assert(!temp);
  }

  assert(bIt == fIt);
  assert((**fIt).getEnergy() <= 0);

  /* Erase.  No memory leak since we are using unique_ptr */
  agents.erase(fIt, agents.end());

  return numDead;
}

/**
 * @brief Executed by a thread during the feeding round
 */
unsigned threadFeeding(const Parameters &params, AgentVector &agents,
                       const AgentVector::iterator &start,
                       const AgentVector::iterator &end) {
  unsigned numDead = 0;

  /* Feeding round */
  AgentVector::iterator a;
  AgentVector::iterator b;
  PredationOutcome outcome;

  for (a = start; a < end; a += 2) {
    b = a + 1;
    outcome = predation(**a, **b, params);
    if (outcome == PREDATION_FIRST_SURVIVES) {
      (**b).setEnergy(0);
      feed(**a, **b, params);
      numDead += 1;
    }

    if (outcome == PREDATION_SECOND_SURVIVES) {
      (**a).setEnergy(0);
      feed(**b, **a, params);
      numDead += 1;
    }
  }

  /* Starvation round */
  for (a = start; a != end; a++) {
    if (starve(**a, params)) {
      (**a).setEnergy(0);
      numDead += 1;
    }
  }

  return numDead;
}

/**
 * @brief Executed by a thread during the mating round
 */
unsigned threadMating(const Parameters &params, AgentVector &agents,
                      const AgentVector::iterator &start,
                      const AgentVector::iterator &end,
                      AgentVector &children) {
  unsigned numBorn = 0;

  /* Mating round */
  AgentVector::iterator a;
  AgentVector::iterator b;

  for (a = start; a < end; a += 2) {
    b = a + 1;
    if (mate(**a, **b, params)) {
      Agent child = crossover(**a, **b, params);
      child.setEnergy(params.lambdaEnergy);
      mutate(child, params);

      /* Store a copy on the heap */
      Agent *c = new Agent(child);
      std::unique_ptr<Agent> ptr(c);
      children.push_back(std::move(ptr));
      numBorn += 1;
    }
  }

  return numBorn;
}

/* -------------------------------------------------------------------------- *
 * Ecosystem class                                                            *
 * -------------------------------------------------------------------------- */

Ecosystem::Ecosystem() { }

Ecosystem::Ecosystem(const Parameters &params) {
  m_parameters = params;

  /* Allocate agents */
  for (unsigned n = 0; n < m_parameters.sizePopulation; n++) {
    Agent *a = new Agent(m_parameters.sizeChromosome, 0x00,
                         m_parameters.lambdaEnergy);
    std::unique_ptr<Agent> agentPtr(a);
    m_agents.push_back(std::move(agentPtr));
  }
}

void Ecosystem::runOnceSerial(void) {

  /* Insert simple (algae) organisms until the population is full */
  for (unsigned n = m_agents.size(); n < m_parameters.sizePopulation; n++) {
    Agent *a = new Agent(m_parameters.sizeChromosome, 0x00,
                         m_parameters.lambdaEnergy);
    std::unique_ptr<Agent> agentPtr(a);
    m_agents.push_back(std::move(agentPtr));
  }

  /* Feeding round */
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(m_agents.begin(), m_agents.end(), g);

  int deltaPopulation = - threadFeeding(m_parameters, m_agents,
                                        m_agents.begin(), m_agents.end());
  removeDeadAgents(m_agents);

  /* Mating round */
  std::shuffle(m_agents.begin(), m_agents.end(), g);
  AgentVector children;

  deltaPopulation = threadMating(m_parameters, m_agents,
                                 m_agents.begin(), m_agents.end(), children);

  /* Can't use vector::insert since this uses a copy instead of move */
  for (unsigned i = 0; i < children.size(); i++) {
    m_agents.push_back(std::move(children[i]));
  }
}

void Ecosystem::run(unsigned numIterations) {
  for (unsigned i = 0; i < numIterations; i++) {
    runOnceSerial();
  }
}



