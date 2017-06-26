#include "common/agent.h"  // pre ae::Agent, ae::Environment
#include "common/agent_body.h"  // pre ae::AgentBody
#include "common/config.h"  // pre ae::config::get
#include "common/timing.h" // pre ae::time::sleep_for, ae::time::milliseconds
#include "common/logging.h" // pre LOG()

#include "tracking.h"
#include "pheromone.h"

//const uint64_t pheromone_gridsize_x = ae::config::get["pheromone"]["pheromone_grid_x"];
//const uint64_t pheromone_gridsize_y = ae::config::get["pheromone"]["pheromone_grid_y"];

std::vector<std::vector<PheromoneParams>> g_pheromone_grid;

TrackingAgent::TrackingAgent(const nlohmann::json &parameters) :
  Agent()
{
  (void) parameters;
  m_interface.position.x = 0;//(rand() % 20) - 10;
  m_interface.position.y = 0;//(rand() % 20) - 10;

  m_interface.color = {0.0f, 1.0f, 1.0f};

  init_pheromone_grid();
  // m_interface.value[0] = typ feromonu;
  // m_interface.value[1] = intenzita;

  //osetrit ci su naozaj
  //TODO
  //m_params_for_pheromones = parameters["pheromone_params"];
  // m_params_for_pheromones["diffusion"];

//TODO
/*
  PheromoneAgent(m_params_for_pheromones, gdfgdf)

  m_interface.timestamp = ae::time::timestamp();
  //m_interface.expires = ae::time::future_timestamp(ae::time::seconds(5));
  */
}
/**
 * [TrackingAgent::add_pheromone_agent]
 * @param position [position of pheromone to create]
 * @param &env [environment to add pheromone on]
 */

void TrackingAgent::add_pheromone_agent (ae::sAgentPosition position, ae::Environment &env)
{
  //set position to tracked agent
  PheromoneAgent *agent_to_add = new PheromoneAgent(position);

  if (agent_to_add == nullptr)
  {
    LOG(ERROR) << "PheromoneAgent allocation error.";
  }
  else
  {
    //agents.push_back(agent);
    LOG(INFO) << "PheromoneAgent Created. Number: ";// << agent_to_add.id;
    env.add_agent(agent_to_add);
    //update grid pheromones ID and exist;
    sPheromoneGridPosition value = agent_to_grid_position(position);
    //TODO solve this problem with protected
    //TODO move it to PheromoneAgent(sAgentInterface.position) constructor]
    //TODO to save ID of created agent so we can easily kill it
    //TODO when pheromone fall under trehold
  //  g_pheromone_grid[value.x][value.y].m_parameters.id = agent_to_add.id;
    g_pheromone_grid[value.x][value.y].m_parameters.alive = true;
  }
}

void TrackingAgent::del_pheromone_agent(sPheromoneGridPosition position, ae::Environment &env)
{

  uint64_t id;
  id = g_pheromone_grid[position.x][position.y].m_parameters.id;

  env.del_agent(id);
  LOG(INFO) << "PheromoneAgent: deleted";
  g_pheromone_grid[position.x][position.y].m_parameters.alive = false;
}

/** \brief Napíše do logu, že  bola zavolaná táto funkcia a čaká 5 ms. */
void TrackingAgent::process(ae::Environment &env)
{
  //(void)env;

  m_interface.timestamp = ae::time::timestamp();
  //m_interface.expires = ae::time::future_timestamp(ae::time::seconds(1));

  //finalne premiestnit do statickej = sucastou triedy
  const uint16_t target_type = ae::config::get["agent_list"]["follower"]["interface_type"];
  //LOG(INFO) << "Target type: " << target_type;
  //cyklus na prejdenie vsetkych agentov co su v systeme
  //m_interface je premenna typu sAgentInterface
  const auto &agent_list = env.global_state();
  //cyklus ktory prejde vsetky prvky "for each"
  //

  //
  for (const ae::sAgentInterface &agent : agent_list)
   {
     //LOG(INFO) << "Searching.";
    /* code */
    //otestujem ci je agent typu real_robot_agent
    if (agent.type == target_type)
    {
      //zistim jeho poziciu
      //
      //vytvorim noveho agenta noveho typu feromon
      //
      //ae::sAgentPosition real_robot_agent_position;

      add_pheromone_agent(agent.position, env);

      increase_pheromon(agent.position);
        //TODO HANDLE AND KILL PHEROMONES
        //TODO checking position
        //TODO hold all active positions with intensity
        //TODO maybe pheromone matrix
        //TODO cycle to check active points and write them
    }
  }

}

void TrackingAgent::init_pheromone_grid()
{
  PheromoneGridSize m_grid_size;
  //LOG(INFO) << "TrackingAgent GridSize X: " << m_grid_size.x << " , Y: " << m_grid_size.y;
  //cout << "TrackingAgent GridSize X: " << m_grid_size.x << " , Y: " m_grid_size.y <<endl;

  g_pheromone_grid.resize(m_grid_size.x);

  for (size_t i = 0; i < g_pheromone_grid.size(); i++)
  {
    g_pheromone_grid[i].resize(m_grid_size.y);
  }
  LOG(INFO) << "PheromoneGrid Allocated ";
}

sPheromoneGridPosition TrackingAgent::agent_to_grid_position(ae::sAgentPosition position)
{
  sPheromoneGridPosition value;
  PheromoneGridSize m_grid_size;

  value.x = position.x + (m_grid_size.x / 2);
  value.y = position.y + (m_grid_size.y / 2);

  return value;
}

ae::sAgentPosition TrackingAgent::grid_to_agent_position(sPheromoneGridPosition position)
{
  ae::sAgentPosition value;
  PheromoneGridSize m_grid_size;

  value.x = position.x - (m_grid_size.x / 2);
  value.y = position.y - (m_grid_size.y / 2);

  return value;
}

void TrackingAgent::increase_pheromon(ae::sAgentPosition position)
{
  sPheromoneGridPosition value;
  value = agent_to_grid_position(position);
  g_pheromone_grid[value.x][value.y].m_parameters.intensity += g_pheromone_grid[value.x][value.y].m_parameters.rise_speed;
  if (g_pheromone_grid[value.x][value.y].m_parameters.intensity > 1)
  {
    g_pheromone_grid[value.x][value.y].m_parameters.intensity = 1;
  }
}

void TrackingAgent::decrease_pheromon()
{
  for (size_t i = 0; i < g_pheromone_grid.size(); i++)
  {
    for (size_t j = 0; j < g_pheromone_grid[i].size(); j++)
    {
      g_pheromone_grid[i][j].m_parameters.intensity -= g_pheromone_grid[i][j].m_parameters.fade_speed;
      if (g_pheromone_grid[i][j].m_parameters.intensity <= 0)
      {
        g_pheromone_grid[i][j].m_parameters.intensity = 0;
      }
    }
  }
}

void TrackingAgent::process_pheromones()
{
  for (size_t i = 0; i < g_pheromone_grid.size(); i++)
  {
    for (size_t j = 0; j < g_pheromone_grid[i].size(); j++)
    {
      //code
    }
  }
}

uint16_t TrackingAgent::assigned_type() const
{
  // plugin.cpp chcecks that this config entry exists
  return ae::config::get["agent_list"]["tracking"]["interface_type"];
}
