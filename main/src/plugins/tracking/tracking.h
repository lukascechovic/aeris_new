#ifndef _TRACKING_AGENT_H_
#define _TRACKING_AGENT_H_

#include <cstdint> // for integer types

#include "common/agent.h" // for ae::Agent, ae::Environment
#include "common/config.h" // for nlohmann::json

class TrackingAgent : public ae::Agent
{
  private:
    nlohmann::json m_params_for_pheromones;

  public:
    TrackingAgent(const nlohmann::json &parameters);
    ~TrackingAgent() {}

    //TODO
    //
    void add_pheromone(ae::sAgentPosition position, ae::Environment &env);

    void process(ae::Environment &env);

    uint16_t assigned_type() const;

    bool remove_protection() const { return false; }
};


#endif /* _PHEROMONE_AGENT_H_ */
