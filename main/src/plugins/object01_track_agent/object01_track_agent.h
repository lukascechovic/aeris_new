#ifndef _OBJECT_01_TRACK_AGENT_H_
#define _OBJECT_01_TRACK_AGENT_H_

#include <cstdint> // for integer types

#include "common/agent.h" // for ae::Agent, ae::Environment
#include "common/config.h" // for nlohmann::json

#include "object01_agent.h" // for sObject01GridPosition

extern std::vector<std::vector<Object01Params>> g_object01_grid;

sObject01GridPosition agent_to_grid_position(ae::sAgentPosition position);
ae::sAgentPosition grid_to_agent_position(sObject01GridPosition position);

class Object01TrackAgent : public ae::Agent
{
  private:
    nlohmann::json m_params_for_object01s;

  public:
    Object01TrackAgent(const nlohmann::json &parameters);
    ~Object01TrackAgent() {}

    //TODO
    void init_object01_grid();
    //Object01Agent
    void add_object01_agent(ae::sAgentPosition position, ae::Environment &env);
    //Object01Agent
    void del_object01_agent(sObject01GridPosition position, ae::Environment &env);

    void increase_object01_intensity(ae::sAgentPosition position);

    void decrease_object01_intensitys();

    void process(ae::Environment &env);

    void process_object01s(ae::Environment &env);

    uint16_t assigned_type() const;

    bool remove_protection() const { return false; }
};

#endif /* _OBJECT_01_TRACK_AGENT_H_ */
