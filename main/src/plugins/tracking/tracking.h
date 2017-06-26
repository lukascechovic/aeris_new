#ifndef _TRACKING_AGENT_H_
#define _TRACKING_AGENT_H_

#include <cstdint> // for integer types

#include "common/agent.h" // for ae::Agent, ae::Environment
#include "common/config.h" // for nlohmann::json

#include "pheromone.h" // for sPheromoneGridPosition

extern std::vector<std::vector<PheromoneParams>> g_pheromone_grid;

class TrackingAgent : public ae::Agent
{
  private:
    nlohmann::json m_params_for_pheromones;

  public:
    TrackingAgent(const nlohmann::json &parameters);
    ~TrackingAgent() {}

    //TODO
    void init_pheromone_grid();

    sPheromoneGridPosition agent_to_grid_position(ae::sAgentPosition position);
    ae::sAgentPosition grid_to_agent_position(sPheromoneGridPosition position);
    //PheromoneAgent
    void add_pheromone_agent(ae::sAgentPosition position, ae::Environment &env);
    //PheromoneAgent
    void del_pheromone_agent(sPheromoneGridPosition position, ae::Environment &env);

    void increase_pheromon(ae::sAgentPosition position);

    void decrease_pheromon();

    void process(ae::Environment &env);

    void process_pheromones();

    uint16_t assigned_type() const;

    bool remove_protection() const { return false; }
};


#endif /* _PHEROMONE_AGENT_H_ */
