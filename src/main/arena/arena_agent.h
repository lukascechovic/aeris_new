#ifndef _ARENA_AGENT_H_
#define _ARENA_AGENT_H_

#include "../common/common.h"

class CArenaAgent: public CAgent
{
  private:
    class CVisualisation *visualisation;

  public:
    CArenaAgent();
    CArenaAgent(struct sAgentInterface agent_interface,
                  class CAgentGroup *agent_group = NULL,
                  unsigned long int group_id = 0);

    ~CArenaAgent();


    unsigned long int get_agent_type();
    void agent_process();

    virtual class CAgent* create(struct sAgentInterface agent_interface, class CAgentGroup *agent_group, unsigned long int group_id);
};



#endif
