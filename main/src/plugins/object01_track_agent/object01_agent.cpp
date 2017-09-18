#include "common/agent.h"  // pre ae::Agent, ae::Environment
#include "common/agent_body.h"  // pre ae::AgentBody
#include "common/config.h"  // pre ae::config::get
#include "common/timing.h" // pre ae::time::sleep_for, ae::time::milliseconds
#include "common/logging.h" // pre LOG()

#include "object01_agent.h"
#include "object01_track_agent.h"

extern std::vector<std::vector<Object01Params>> g_object01_grid;

Object01Agent::Object01Agent() :
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

Object01Agent::Object01Agent(float x, float y) :
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
  m_interface.type = ae::config::get["agent_list"]["object01_agent"]["interface_type"];
  m_interface.expires = ae::time::future_timestamp(ae::time::seconds(1));
}

Object01Agent::Object01Agent(ae::sAgentPosition position):
  Agent()
{
  //TODO be able to chage body
  //TODO atm BUG getting body from Other plugin of same class name
  //TODO Object01Agent plugin
  //m_interface.body = 1;

  m_interface.position.x = position.x;
  m_interface.position.y = position.y;
  //m_interface.position.z = -5;
  //m_interface.position.x = (rand() % 20) - 10;
  //m_interface.position.y = (rand() % 20) - 10;

  //m_interface.color = {0.0f, 1.0f, 0.5f};
  //SET UP DEFAULT COLOR
  //maximum color intensity
  m_interface.color.r = ae::config::get["object01_track_agent"]["r"];
  m_interface.color.g = ae::config::get["object01_track_agent"]["g"];
  m_interface.color.b = ae::config::get["object01_track_agent"]["b"];
  //starting color intensity
  float rise_from = ae::config::get["object01_track_agent"]["rise_from"];
  m_interface.color.r *= rise_from;
  m_interface.color.g *= rise_from;
  m_interface.color.b *= rise_from;

  //TODO read type
  m_interface.type = 33;
  // m_interface.value[0] = typ feromonu;
  // m_interface.value[1] = intenzita;
  m_interface.timestamp = ae::time::timestamp();
  //m_interface.type = ae::config::get["agent_list"]["object01_track_agent"]["interface_type"];
  m_interface.expires = ae::time::future_timestamp(ae::time::seconds(1));

  //update grid object01s ID and exist;
  sObject01GridPosition value = agent_to_grid_position(position);

  //m_interface.id is seted up after contructor ends by systeme
  //so we are saving position coordinates with agent and then
  //in Object01Agent.process pass agents id to g_object01_grid structure
  //to have correct id on correct postion to be able kill Object01Agent's
  //using ID
  //g_object01_grid[value.x][value.y].m_parameters.id = m_interface.id;
  g_object01_grid[value.x][value.y].m_parameters.alive = true;
  //save grid position of this agent
  m_grid_position.x = value.x;
  m_grid_position.y = value.y;
}

/** \brief Napíše do logu, že  bola zavolaná táto funkcia a čaká 5 ms. */
void Object01Agent::process(ae::Environment &env)
{
  (void)env;

  //json body_list interface_type number
  // m_interface.body = 3; plane_1cm
  m_interface.body = 3;

  g_object01_grid[m_grid_position.x][m_grid_position.y].m_parameters.id = m_interface.id;
  //LOG(INFO) << "Object01Agent: Process, ID = " << m_interface.id;

  m_interface.timestamp = ae::time::timestamp();
  m_interface.expires = ae::time::future_timestamp(ae::time::seconds(1));
/*
  //old killing mechanism
  uint64_t object01_expires = 15000000;
  if ( (m_interface.timestamp+object01_expires) < ae::time::timestamp() )
  {
    env.del_agent(m_interface.id);
    LOG(INFO) << "Object01Agent: deleted";
  }
*/

  //SET UP DEFAULT COLOR
  //maximum color intensity
  m_interface.color.r = ae::config::get["object01_track_agent"]["r"];
  m_interface.color.g = ae::config::get["object01_track_agent"]["g"];
  m_interface.color.b = ae::config::get["object01_track_agent"]["b"];
  //UPDATE actual INTENSITY
  m_interface.color.r *= g_object01_grid[m_grid_position.x][m_grid_position.y].m_parameters.intensity;
  m_interface.color.g *= g_object01_grid[m_grid_position.x][m_grid_position.y].m_parameters.intensity;
  m_interface.color.b *= g_object01_grid[m_grid_position.x][m_grid_position.y].m_parameters.intensity;

}

uint16_t Object01Agent::assigned_type() const
{
  // plugin.cpp chcecks that this config entry exists
  return ae::config::get["agent_list"]["object01_track_agent"]["interface_type"];
}

Object01Params::Object01Params()
{
  m_parameters.id = 0;
  m_parameters.intensity = 0;
  m_parameters.fade_speed = ae::config::get["object01_track_agent"]["fade_speed"];
  m_parameters.rise_speed = ae::config::get["object01_track_agent"]["rise_speed"];
  m_parameters.alive = false;
  m_parameters.threshold = ae::config::get["object01_track_agent"]["threshold"];
}

Object01GridSize::Object01GridSize()
{
  m_gridsize.x = ae::config::get["object01_track_agent"]["object01_grid_x"];
  x = ae::config::get["object01_track_agent"]["object01_grid_x"];

  m_gridsize.y = ae::config::get["object01_track_agent"]["object01_grid_y"];
  y = ae::config::get["object01_track_agent"]["object01_grid_y"];
}
