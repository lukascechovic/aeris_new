#include "common/agent.h"  // pre ae::Agent, ae::Environment
#include "common/agent_body.h"  // pre ae::AgentBody
#include "common/config.h"  // pre ae::config::get
#include "common/timing.h" // pre ae::time::sleep_for, ae::time::milliseconds
#include "common/logging.h" // pre LOG()

#include "pheromone.h"
#include "tracking.h"


PheromoneAgent::PheromoneAgent() :
  Agent()
{
  m_interface.position.x = (rand() % 20) - 10;
  m_interface.position.y = (rand() % 20) - 10;

  m_interface.color = {0.0f, 0.5f, 0.5f};
  // m_interface.value[0] = typ feromonu;
  // m_interface.value[1] = intenzita;



  m_interface.timestamp = ae::time::timestamp();
  m_interface.expires = ae::time::future_timestamp(ae::time::seconds(5));
}

PheromoneAgent::PheromoneAgent(float x, float y) :
  Agent()
{
  m_interface.position.x = x;
  m_interface.position.y = y;
  //m_interface.position.x = (rand() % 20) - 10;
  //m_interface.position.y = (rand() % 20) - 10;

  m_interface.color = {0.0f, 1.0f, 0.5f};
  // m_interface.value[0] = typ feromonu;
  // m_interface.value[1] = intenzita;
  m_interface.timestamp = ae::time::timestamp();
  //m_interface.type = ae::config::get["agent_list"]["pheromone"]["interface_type"];
  m_interface.expires = ae::time::future_timestamp(ae::time::seconds(5));
}

PheromoneAgent::PheromoneAgent(ae::sAgentPosition position):
  Agent()
{
  m_interface.position.x = position.x;
  m_interface.position.y = position.y;
  //m_interface.position.x = (rand() % 20) - 10;
  //m_interface.position.y = (rand() % 20) - 10;

  m_interface.color = {0.0f, 1.0f, 0.5f};
  m_interface.type = 22;
  // m_interface.value[0] = typ feromonu;
  // m_interface.value[1] = intenzita;
  m_interface.timestamp = ae::time::timestamp();
  //m_interface.type = ae::config::get["agent_list"]["pheromone"]["interface_type"];
  m_interface.expires = ae::time::future_timestamp(ae::time::seconds(5));

/*
  //update grid pheromones ID and exist;
  sPheromoneGridPosition value = agent_to_grid_position(position);
  g_pheromone_grid[value.x][value.y].m_parameters.id = m_interface.id;
  g_pheromone_grid[value.x][value.y].m_parameters.alive = true;
*/
}


/** \brief Napíše do logu, že  bola zavolaná táto funkcia a čaká 5 ms. */
void PheromoneAgent::process(ae::Environment &env)
{
  (void)env;

  //5000000 = 5 seconds
  //TODO rework
  uint64_t pheromone_expires = 15000000;
  if ( (m_interface.timestamp+pheromone_expires) < ae::time::timestamp() )
  {
    env.del_agent(m_interface.id);
    LOG(INFO) << "PheromoneAgent: deleted";
  }

  //std::cout << "time " << m_interface.timestamp << " : " << ae::time::timestamp() << std::endl;

  //m_interface.timestamp = ae::time::timestamp();
  //m_interface.expires = ae::time::future_timestamp(ae::time::seconds(1));

  //finalne premiestnit do statickej = sucastou triedy
  const uint16_t target_type = ae::config::get["agent_list"]["follower"]["interface_type"];
  //cyklus na prejdenie vsetkych agentov co su v systeme
  //m_interface je premenna typu sAgentInterface, this na process agent
  const auto &agent_list = env.global_state();
  //cyklus ktory prejde vsetky prvky "for each"
  for (const ae::sAgentInterface &agent : agent_list)
   {
    /* code */
    if (agent.type == target_type)
    {
//
    }
  }

}

uint16_t PheromoneAgent::assigned_type() const
{
  // plugin.cpp chcecks that this config entry exists
  return ae::config::get["agent_list"]["pheromone"]["interface_type"];
}
/*
sPheromoneGridPosition PheromoneAgent::agent_to_grid_position(ae::sAgentPosition position)
{
  sPheromoneGridPosition value;
  PheromoneGridSize m_grid_size;

  value.x = position.x + (m_grid_size.x / 2);
  value.y = position.y + (m_grid_size.y / 2);

  return value;
}

ae::sAgentPosition PheromoneAgent::grid_to_agent_position(sPheromoneGridPosition position)
{
  ae::sAgentPosition value;
  PheromoneGridSize m_grid_size;

  value.x = position.x - (m_grid_size.x / 2);
  value.y = position.y - (m_grid_size.y / 2);

  return value;
}
*/
PheromoneParams::PheromoneParams()
{
  m_parameters.id = 0;
  m_parameters.intensity = 0;
  m_parameters.fade_speed = ae::config::get["pheromone"]["fade_speed"];
  m_parameters.rise_speed = ae::config::get["pheromone"]["rise_speed"];
  m_parameters.alive = false;
}

PheromoneGridSize::PheromoneGridSize()
{
  m_gridsize.x = ae::config::get["pheromone"]["pheromone_grid_x"];
  x = ae::config::get["pheromone"]["pheromone_grid_x"];

  m_gridsize.y = ae::config::get["pheromone"]["pheromone_grid_y"];
  y = ae::config::get["pheromone"]["pheromone_grid_y"];
}
