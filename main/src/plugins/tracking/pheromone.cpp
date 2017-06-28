#include "common/agent.h"  // pre ae::Agent, ae::Environment
#include "common/agent_body.h"  // pre ae::AgentBody
#include "common/config.h"  // pre ae::config::get
#include "common/timing.h" // pre ae::time::sleep_for, ae::time::milliseconds
#include "common/logging.h" // pre LOG()

#include "pheromone.h"
#include "tracking.h"

extern std::vector<std::vector<PheromoneParams>> g_pheromone_grid;

PheromoneAgent::PheromoneAgent() :
  Agent()
{
  m_interface.position.x = (rand() % 20) - 10;
  m_interface.position.y = (rand() % 20) - 10;

  m_interface.color = {0.0f, 0.5f, 0.5f};
  // m_interface.value[0] = typ feromonu;
  // m_interface.value[1] = intenzita;



  m_interface.timestamp = ae::time::timestamp();
  m_interface.expires = ae::time::future_timestamp(ae::time::seconds(1));
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
  m_interface.expires = ae::time::future_timestamp(ae::time::seconds(1));
}

PheromoneAgent::PheromoneAgent(ae::sAgentPosition position):
  Agent()
{
  m_interface.position.x = position.x;
  m_interface.position.y = position.y;
  //m_interface.position.z = -5;
  //m_interface.position.x = (rand() % 20) - 10;
  //m_interface.position.y = (rand() % 20) - 10;

  //m_interface.color = {0.0f, 1.0f, 0.5f};
  m_interface.color.r = ae::config::get["pheromone"]["r"];
  m_interface.color.g = ae::config::get["pheromone"]["g"];
  m_interface.color.b = ae::config::get["pheromone"]["b"];
  m_interface.type = 22;
  // m_interface.value[0] = typ feromonu;
  // m_interface.value[1] = intenzita;
  m_interface.timestamp = ae::time::timestamp();
  //m_interface.type = ae::config::get["agent_list"]["pheromone"]["interface_type"];
  m_interface.expires = ae::time::future_timestamp(ae::time::seconds(1));

  //update grid pheromones ID and exist;
  sPheromoneGridPosition value = agent_to_grid_position(position);
  //TODO generating ID which can not be mistaken with other
  //TODO causing error
  //m_interface.id = value.x*100+value.y;
  m_grid_position.x = value.x;
  m_grid_position.y = value.y;
  //m_interface.id is seted up after contructor ends by systeme
  //so we are saving position coordinates with agent and then
  //in PheromoneAgent.process pass agents id to g_pheromone_grid structure
  //to have correct id on correct postion to be able kill PheromoneAgent's
  //using ID
  //g_pheromone_grid[value.x][value.y].m_parameters.id = m_interface.id;
  g_pheromone_grid[value.x][value.y].m_parameters.alive = true;
  //save grid position of this agent
  m_grid_position.x = value.x;
  m_grid_position.y = value.y;

}


/** \brief Napíše do logu, že  bola zavolaná táto funkcia a čaká 5 ms. */
void PheromoneAgent::process(ae::Environment &env)
{
  (void)env;

  g_pheromone_grid[m_grid_position.x][m_grid_position.y].m_parameters.id = m_interface.id;
  //LOG(INFO) << "PheromoneAgent: Process, ID = " << m_interface.id;

  m_interface.timestamp = ae::time::timestamp();
  m_interface.expires = ae::time::future_timestamp(ae::time::seconds(1));
/*
  //old killing mechanism
  uint64_t pheromone_expires = 15000000;
  if ( (m_interface.timestamp+pheromone_expires) < ae::time::timestamp() )
  {
    env.del_agent(m_interface.id);
    LOG(INFO) << "PheromoneAgent: deleted";
  }
*/

  //SET UP DEFAULT COLOR
  m_interface.color.r = ae::config::get["pheromone"]["r"];
  m_interface.color.g = ae::config::get["pheromone"]["g"];
  m_interface.color.b = ae::config::get["pheromone"]["b"];
  //UPDATE INTENSITY
  m_interface.color.r *= g_pheromone_grid[m_grid_position.x][m_grid_position.y].m_parameters.intensity;
  m_interface.color.g *= g_pheromone_grid[m_grid_position.x][m_grid_position.y].m_parameters.intensity;
  m_interface.color.b *= g_pheromone_grid[m_grid_position.x][m_grid_position.y].m_parameters.intensity;

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
  m_parameters.threshold = ae::config::get["pheromone"]["threshold"];
}

PheromoneGridSize::PheromoneGridSize()
{
  m_gridsize.x = ae::config::get["pheromone"]["pheromone_grid_x"];
  x = ae::config::get["pheromone"]["pheromone_grid_x"];

  m_gridsize.y = ae::config::get["pheromone"]["pheromone_grid_y"];
  y = ae::config::get["pheromone"]["pheromone_grid_y"];
}
