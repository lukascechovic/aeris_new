#include "../common/common.h"

int main()
{
  struct sAgentGroupInitStruct agent_group_init_struct;

  agent_group_init_struct.count = 100;
  agent_group_init_struct.random_positions = true;
  agent_group_init_struct.dt = 50.0;

  class CClient *client;



  class CExampleAgent *agent = new CExampleAgent();
  //class CAgent *agent = new CAgent();


  client = new CClient(agent_group_init_struct, agent);
  client->main();


  delete client;


  printf("program done\n");
  return 0;
}
