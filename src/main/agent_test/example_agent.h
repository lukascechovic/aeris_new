#ifndef _EXAMPLE_AGENT_H_
#define _EXAMPLE_AGENT_H_

#include "../common/common.h"

class CExampleAgent: public CAgent
{
  public:
    CExampleAgent();
    CExampleAgent(struct sAgentInterface agent_interface,
                  class CAgentGroup *agent_group = NULL,
                  unsigned long int group_id = 0);

    ~CExampleAgent();


    unsigned long int get_agent_type();

    void agent_process();

    virtual class CAgent* create(struct sAgentInterface agent_interface, class CAgentGroup *agent_group, unsigned long int group_id);
};


#endif
