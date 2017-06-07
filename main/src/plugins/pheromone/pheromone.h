#ifndef _PHEROMONE_AGENT_H_
#define _PHEROMONE_AGENT_H_

#include <cstdint> // for integer types

#include "common/agent.h" // for ae::Agent, ae::Environment
#include "common/config.h" // for nlohmann::json

class PheromoneAgent : public ae::Agent
{
  private:

  public:
    PheromoneAgent(const nlohmann::json &parameters);
    ~PheromoneAgent() {}

    void process(ae::Environment &env);

    uint16_t assigned_type() const;

    bool remove_protection() const { return false; }
};


#endif /* _PHEROMONE_AGENT_H_ */
