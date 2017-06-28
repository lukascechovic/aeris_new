#include "common/agent.h"  // pre ae::Agent, ae::Environment
#include "common/agent_body.h"  // pre ae::AgentBody
#include "common/config.h"  // pre ae::config::get
#include "common/timing.h" // pre ae::time::sleep_for, ae::time::milliseconds
#include "common/logging.h" // pre LOG()

#include "follower.h"


FollowerAgent::FollowerAgent(const nlohmann::json &parameters) :
  Agent()
{
  (void) parameters;
  //m_interface.position.x = (rand() % 20) - 10;
  //m_interface.position.y = (rand() % 20) - 10;
  //
  int x = ae::config::get["pheromone"]["pheromone_grid_x"];
  int y = ae::config::get["pheromone"]["pheromone_grid_y"];

  m_interface.position.x = ceil( (rand() % x) - (x/2) );
  m_interface.position.y = ceil( (rand() % y) - (y/2) );
  //m_interface.position.z = -0.9;

  m_interface.color = {0.0f, 1.0f, 0.0f};
  // m_interface.value[0] = typ feromonu;
  // m_interface.value[1] = intenzita;



  m_interface.timestamp = ae::time::timestamp();
  m_interface.expires = ae::time::future_timestamp(ae::time::seconds(1));
}

/** \brief Napíše do logu, že  bola zavolaná táto funkcia a čaká 5 ms. */
void FollowerAgent::process(ae::Environment &env)
{
  (void)env;

  int x = ae::config::get["pheromone"]["pheromone_grid_x"];
  int y = ae::config::get["pheromone"]["pheromone_grid_y"];

  m_interface.position.x = ceil( (rand() % x) - (x/2) );
  m_interface.position.y = ceil( (rand() % y) - (y/2) );
  //LOG(INFO) << "FollowerAgent Position: " << m_interface.position.x << " " <<m_interface.position.y;
  //LOG(INFO) << "FollowerAgent Moved.";
  m_interface.timestamp = ae::time::timestamp();
  m_interface.expires = ae::time::future_timestamp(ae::time::seconds(1));

  //finalne premiestnit do statickej = sucastou triedy
  const uint16_t target_type = ae::config::get["agent_list"]["follower"]["interface_type"];


  //cyklus na prejdenie vsetkych agentov co su v systeme
  //m_interface je premenna typu sAgentInterface
  const auto &agent_list = env.global_state();
  //cyklus ktory prejde vsetky prvky "for each"
  for (const ae::sAgentInterface &agent : agent_list)
   {
    /* code */
    //otestujem ci je agent typu real_robot_agent
    ////TODO Lukas Dopisal
    ////TODO Pozriet a vyuzit kontrolu ID laebo inych parametrov najdeneho robota
    //if (agent.type == target_type && agent.id != m_interface.id)
    if (agent.type == target_type)
    {

    }
  }

}


uint16_t FollowerAgent::assigned_type() const
{
  // plugin.cpp chcecks that this config entry exists
  return ae::config::get["agent_list"]["follower"]["interface_type"];
}
/*
uint16_t PheromoneAgent::assigned_type() const
{
  // plugin.cpp chcecks that this config entry exists
  return ae::config::get["agent_list"]["follower"]["interface_type"];
}
*/
