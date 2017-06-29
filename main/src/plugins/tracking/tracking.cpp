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
    //LOG(INFO) << "PheromoneAgent Created. Position: " << position.x << " " <<position.y;
    env.add_agent(agent_to_add);
  }
}

//TODO not working because of wrong ID!!!!!!!
void TrackingAgent::del_pheromone_agent(sPheromoneGridPosition position, ae::Environment &env)
{

  uint64_t id;
  id = g_pheromone_grid[position.x][position.y].m_parameters.id;

  env.del_agent(id);
  //LOG(INFO) << "PheromoneAgent: deleted, ID = " << id;
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
  for (const ae::sAgentInterface &agent : agent_list)
   {
     //LOG(INFO) << "Searching.";
    /* code */
    //otestujem ci je agent typu real_robot_agent
    if (agent.type == target_type)
    {
      //LOG(INFO) << "FollowerAgent Position Read: " << agent.position.x << " " <<agent.position.y;
      //increase_pheromon on position of agent to track
      increase_pheromon(agent.position);
    }
  }
  //decrease_pheromons on all grid positions
  decrease_pheromons();
  //kill and create PheromoneAgent's base on intensity in grid matrix
  process_pheromones(env);

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

sPheromoneGridPosition agent_to_grid_position(ae::sAgentPosition position)
{
  sPheromoneGridPosition value;
  PheromoneGridSize m_grid_size;

  value.x = position.x + (m_grid_size.x / 2.0);
  value.y = position.y + (m_grid_size.y / 2.0);

  return value;
}

ae::sAgentPosition grid_to_agent_position(sPheromoneGridPosition position)
{
  ae::sAgentPosition value;
  PheromoneGridSize m_grid_size;

  value.x = (float)position.x - (m_grid_size.x / 2.0);
  value.y = (float)position.y - (m_grid_size.y / 2.0);

  return value;
}

void TrackingAgent::increase_pheromon(ae::sAgentPosition position)
{
  float rise_from = ae::config::get["pheromone"]["rise_from"];
  sPheromoneGridPosition value;
  value = agent_to_grid_position(position);
  //TODO test if value is in range of grid
  int grid_x_size = ae::config::get["pheromone"]["pheromone_grid_x"];
  int grid_y_size = ae::config::get["pheromone"]["pheromone_grid_y"];
  if (((value.x>=0)&&(value.x<grid_x_size))&&((value.y>=0)&&(value.y<grid_y_size)))
  {
    //test Agent alive
    //if dead set up intensity = rise_from
    if (g_pheromone_grid[value.x][value.y].m_parameters.alive == false)
    {
      //SETUP intensity on creation
      g_pheromone_grid[value.x][value.y].m_parameters.intensity = rise_from;
    }
    //if alive increase_pheromon by rise_speed
    else
    {
      //pheromone is under rise_from so renew it
      if (g_pheromone_grid[value.x][value.y].m_parameters.intensity < rise_from)
      {
        g_pheromone_grid[value.x][value.y].m_parameters.intensity = rise_from;
      }
      //pheromone is over rise_from so add rise_speed
      else
      {
        g_pheromone_grid[value.x][value.y].m_parameters.intensity += g_pheromone_grid[value.x][value.y].m_parameters.rise_speed;
        if (g_pheromone_grid[value.x][value.y].m_parameters.intensity > 1)
        {
          g_pheromone_grid[value.x][value.y].m_parameters.intensity = 1;
        }
      }
    }
  }
  else
  {
    LOG(INFO) << "TrackedAgent out of range of GRID";
  }
}

void TrackingAgent::decrease_pheromons()
{
  for (size_t i = 0; i < g_pheromone_grid.size(); i++)
  {
    for (size_t j = 0; j < g_pheromone_grid[i].size(); j++)
    {
      //TODO zmena charakteru vyparovania z JSON?
      g_pheromone_grid[i][j].m_parameters.intensity *= g_pheromone_grid[i][j].m_parameters.fade_speed;
      //g_pheromone_grid[i][j].m_parameters.intensity -= g_pheromone_grid[i][j].m_parameters.fade_speed;

      if (g_pheromone_grid[i][j].m_parameters.intensity <= g_pheromone_grid[i][j].m_parameters.threshold)
      {
        g_pheromone_grid[i][j].m_parameters.intensity = 0.0;
      }
    }
  }
}

void TrackingAgent::process_pheromones(ae::Environment &env)
{
  //grid position
  sPheromoneGridPosition grid_position;
  //agent position
  ae::sAgentPosition agent_position;

  for (size_t i = 0; i < g_pheromone_grid.size(); i++)
  {
    for (size_t j = 0; j < g_pheromone_grid[i].size(); j++)
    {
      //delete PheromoneAgents if under threshold and alive
      if ((g_pheromone_grid[i][j].m_parameters.intensity < g_pheromone_grid[i][j].m_parameters.threshold)
      && (g_pheromone_grid[i][j].m_parameters.alive == true))
      {
        grid_position.x = i;
        grid_position.y = j;
        del_pheromone_agent(grid_position, env);
      }
      //create PheromoneAgents if meets threshold and not alive
      if ((g_pheromone_grid[i][j].m_parameters.intensity >= g_pheromone_grid[i][j].m_parameters.threshold)
      && (g_pheromone_grid[i][j].m_parameters.alive == false))
      {
        grid_position.x = i;
        grid_position.y = j;
        agent_position = grid_to_agent_position(grid_position);
        add_pheromone_agent(agent_position, env);
      }
      //print matrix of intesity
      //std::cout << " "<< std::setprecision(1) << g_pheromone_grid[i][j].m_parameters.intensity;
    }
    //std::cout << std::endl;
  }
  //std::cout << std::endl;
}

uint16_t TrackingAgent::assigned_type() const
{
  // plugin.cpp chcecks that this config entry exists
  return ae::config::get["agent_list"]["tracking"]["interface_type"];
}

//***testing position conversion
/*      //grid position
      sPheromoneGridPosition grid_position;
      //agent position
      ae::sAgentPosition agent_position = agent.position;
      std::cout << "1. x: " << agent.position.x << " y: " << agent.position.y << std::endl;
      grid_position = agent_to_grid_position(agent.position);
      std::cout << "2. x: " << grid_position.x << " y: " << grid_position.y << std::endl;
      agent_position = grid_to_agent_position(grid_position);
      std::cout << "3. x: " << agent_position.x << " y: " << agent_position.y << std::endl;
*/
//****
