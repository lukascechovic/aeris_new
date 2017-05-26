#ifndef _EXAMPLE_AGENT_H_
#define _EXAMPLE_AGENT_H_

#include <cstdint> // for integer types

#include "common/agent.h" // for ae::Agent, ae::Environment
#include "common/config.h" // for nlohmann::json

class FollowerAgent : public ae::Agent
{
  private:

  public:
    FollowerAgent(const nlohmann::json &parameters);
    ~FollowerAgent() {}

    void process(ae::Environment &env);

    uint16_t assigned_type() const;

    bool remove_protection() const { return false; }
};


#endif /* _EXAMPLE_AGENT_H_ */
