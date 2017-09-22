#include "common/agent.h"  // pre ae::Agent, ae::Environment
#include "common/agent_body.h"  // pre ae::AgentBody
#include "common/config.h"  // pre ae::config::get
#include "common/timing.h" // pre ae::time::sleep_for, ae::time::milliseconds
#include "common/logging.h" // pre LOG()

#include "object01_track_agent.h"
#include "object01_agent.h"

//const uint64_t object01_gridsize_x = ae::config::get["object01_track_agent"]["object01_grid_x"];
//const uint64_t object01_gridsize_y = ae::config::get["object01_track_agent"]["object01_grid_y"];

std::vector<std::vector<Object01Params>> g_object01_grid;

Object01TrackAgent::Object01TrackAgent(const nlohmann::json &parameters) :
  Agent()
{
  (void) parameters;
  m_interface.position.x = 0;//(rand() % 20) - 10;
  m_interface.position.y = 0;//(rand() % 20) - 10;

  m_interface.color = {0.0f, 1.0f, 1.0f};

  //handling clear touch on/off information
  //on/off flag
  m_interface.value[0] = 0;

  init_object01_grid();
  // m_interface.value[0] = typ feromonu;
  // m_interface.value[1] = intenzita;

  //osetrit ci su naozaj
  //TODO
  //m_params_for_object01s = parameters["object01_params"];
  // m_params_for_object01s["diffusion"];

//TODO
/*
  Object01Agent(m_params_for_object01s, gdfgdf)

  m_interface.timestamp = ae::time::timestamp();
  //m_interface.expires = ae::time::future_timestamp(ae::time::seconds(5));
  */
}
/**
 * [Object01TrackAgent::add_object01_agent]
 * @param position [position of object01 to create]
 * @param &env [environment to add object01 on]
 */

void Object01TrackAgent::add_object01_agent (ae::sAgentPosition position, ae::Environment &env)
{

  //set position to tracked agent
  Object01Agent *agent_to_add = new Object01Agent(position);

  if (agent_to_add == nullptr)
  {
    LOG(ERROR) << "Object01Agent allocation error.";
  }
  else
  {
    //LOG(INFO) << "Object01Agent Created. Position: " << position.x << " " <<position.y;
    env.add_agent(agent_to_add);
  }
}

//TODO not working because of wrong ID!!!!!!!
void Object01TrackAgent::del_object01_agent(sObject01GridPosition position, ae::Environment &env)
{

  uint64_t id;
  id = g_object01_grid[position.x][position.y].m_parameters.id;

  env.del_agent(id);
  //LOG(INFO) << "Object01Agent: deleted, ID = " << id;
  g_object01_grid[position.x][position.y].m_parameters.alive = false;
}

/** \brief Napíše do logu, že  bola zavolaná táto funkcia a čaká 5 ms. */
void Object01TrackAgent::process(ae::Environment &env)
{
  //(void)env;

  m_interface.timestamp = ae::time::timestamp();
  //m_interface.expires = ae::time::future_timestamp(ae::time::seconds(1));

  //finalne premiestnit do statickej = sucastou triedy
  const uint16_t target_type = ae::config::get["agent_list"]["visualisation"]["interface_type"];
  const uint16_t target_type_2 = ae::config::get["agent_list"]["visualisation"]["interface_type"];
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
    if ((agent.type == target_type)||(agent.type == target_type_2))
    {
      //LOG(INFO) << "FollowerAgent Position Read: " << agent.position.x << " " <<agent.position.y;
      //increase_object01_intensity on position of agent to track
      //clear object intensity if clearing is on
      if (m_interface.value[0] == 1)
      {
        clear_object01_intensity(agent.position);
      }
      else
      {
        increase_object01_intensity(agent.position);
      }

      //clear grid if touch on position [-25,-13]
      //left down corner
      if (agent.position.x <= -25 && agent.position.y <= -13)
      {
        clear_grid_object01();
        m_interface.value[0] = 0;
      }

      //clearing touch on
      //information is handled through m_interface.value[0]
      //left up corner
      if (agent.position.x <= -25 && agent.position.y >= 13)
      {
        m_interface.value[0] = 1;
      }
      //clearing touch off
      //right up corner
      if (agent.position.x >= 25 && agent.position.y >= 13)
      {
        m_interface.value[0] = 0;
      }
    }
  }
  //decrease_object01_intensitys on all grid positions
  decrease_object01_intensitys();
  //kill and create Object01Agent's base on intensity in grid matrix
  process_object01s(env);


}

void Object01TrackAgent::init_object01_grid()
{
  Object01GridSize m_grid_size;
  //LOG(INFO) << "Object01TrackAgent GridSize X: " << m_grid_size.x << " , Y: " << m_grid_size.y;
  //cout << "Object01TrackAgent GridSize X: " << m_grid_size.x << " , Y: " m_grid_size.y <<endl;

  g_object01_grid.resize(m_grid_size.x);

  for (size_t i = 0; i < g_object01_grid.size(); i++)
  {
    g_object01_grid[i].resize(m_grid_size.y);
  }
  LOG(INFO) << "Object01Grid Allocated ";
}

sObject01GridPosition agent_to_grid_position(ae::sAgentPosition position)
{
  sObject01GridPosition value;
  Object01GridSize m_grid_size;

  value.x = position.x + (m_grid_size.x / 2.0);
  value.y = position.y + (m_grid_size.y / 2.0);

  return value;
}

ae::sAgentPosition grid_to_agent_position(sObject01GridPosition position)
{
  ae::sAgentPosition value;
  Object01GridSize m_grid_size;

  value.x = (float)position.x - (m_grid_size.x / 2.0);
  value.y = (float)position.y - (m_grid_size.y / 2.0);

  return value;
}

void Object01TrackAgent::increase_object01_intensity(ae::sAgentPosition position)
{
  float rise_from = ae::config::get["object01_track_agent"]["rise_from"];
  sObject01GridPosition value;
  value = agent_to_grid_position(position);
  //TODO test if value is in range of grid
  int grid_x_size = ae::config::get["object01_track_agent"]["object01_grid_x"];
  int grid_y_size = ae::config::get["object01_track_agent"]["object01_grid_y"];
  if (((value.x>=0)&&(value.x<grid_x_size))&&((value.y>=0)&&(value.y<grid_y_size)))
  {
    //test Agent alive
    if (g_object01_grid[value.x][value.y].m_parameters.alive == false)
    //if no object01 in area set up starting intensity = rise_from
    {
      g_object01_grid[value.x][value.y].m_parameters.intensity = rise_from;
    }
    else
    //if alive (=there is object01 in are before) increase_object01_intensity by rise_speed
    {
      //object01 is under rise_from so renew it on rise
      if (g_object01_grid[value.x][value.y].m_parameters.intensity < rise_from)
      {
        //renew to rise from if intensity is under rise_from
        //g_object01_grid[value.x][value.y].m_parameters.intensity = rise_from;
        //add rise_speed
        g_object01_grid[value.x][value.y].m_parameters.intensity += g_object01_grid[value.x][value.y].m_parameters.rise_speed;
      }
      //object01 is over rise_from so add rise_speed
      else
      {
        g_object01_grid[value.x][value.y].m_parameters.intensity += g_object01_grid[value.x][value.y].m_parameters.rise_speed;
        if (g_object01_grid[value.x][value.y].m_parameters.intensity > 1)
        {
          g_object01_grid[value.x][value.y].m_parameters.intensity = 1;
        }
      }
    }
  }
  else
  {
    if (ae::config::get["object01_track_agent"]["out_of_grid_log_on"] == true)
      LOG(INFO) << "TrackedAgent out of range of GRID";
  }
}

void Object01TrackAgent::clear_object01_intensity(ae::sAgentPosition position)
{
  sObject01GridPosition value;
  value = agent_to_grid_position(position);
  //TODO test if value is in range of grid
  int grid_x_size = ae::config::get["object01_track_agent"]["object01_grid_x"];
  int grid_y_size = ae::config::get["object01_track_agent"]["object01_grid_y"];
  if (((value.x>=0)&&(value.x<grid_x_size))&&((value.y>=0)&&(value.y<grid_y_size)))
  {
      g_object01_grid[value.x][value.y].m_parameters.intensity = 0;
  }
  else
  {
    if (ae::config::get["object01_track_agent"]["out_of_grid_log_on"] == true)
      LOG(INFO) << "TrackedAgent out of range of GRID";
  }
}

void Object01TrackAgent::decrease_object01_intensitys()
{
  for (size_t i = 0; i < g_object01_grid.size(); i++)
  {
    for (size_t j = 0; j < g_object01_grid[i].size(); j++)
    {
      //TODO zmena charakteru vyparovania z JSON?
      g_object01_grid[i][j].m_parameters.intensity *= g_object01_grid[i][j].m_parameters.fade_speed;
      //g_object01_grid[i][j].m_parameters.intensity -= g_object01_grid[i][j].m_parameters.fade_speed;

      if (g_object01_grid[i][j].m_parameters.intensity <= g_object01_grid[i][j].m_parameters.threshold)
      {
        g_object01_grid[i][j].m_parameters.intensity = 0.0;
      }
    }
  }
}

void Object01TrackAgent::clear_grid_object01()
{
  for (size_t i = 0; i < g_object01_grid.size(); i++)
  {
    for (size_t j = 0; j < g_object01_grid[i].size(); j++)
    {
        g_object01_grid[i][j].m_parameters.intensity = 0.0;
    }
  }
}

void Object01TrackAgent::process_object01s(ae::Environment &env)
{
  //number or active object01_intensitys from grid
  int active_object01_value = 0;
  //grid position
  sObject01GridPosition grid_position;
  //agent position
  ae::sAgentPosition agent_position;

  for (size_t i = 0; i < g_object01_grid.size(); i++)
  {
    for (size_t j = 0; j < g_object01_grid[i].size(); j++)
    {
      //counts active object01 grid positions if active_log_on == true
      if ((ae::config::get["object01_track_agent"]["active_log_on"] == true)
        &&(g_object01_grid[i][j].m_parameters.alive == true))
      {
        active_object01_value++;
      }
      //delete Object01Agents if under threshold and alive
      if ((g_object01_grid[i][j].m_parameters.intensity < g_object01_grid[i][j].m_parameters.threshold)
      && (g_object01_grid[i][j].m_parameters.alive == true))
      {
        grid_position.x = i;
        grid_position.y = j;
        del_object01_agent(grid_position, env);
      }
      //create Object01Agents if meets threshold and not alive
      if ((g_object01_grid[i][j].m_parameters.intensity >= g_object01_grid[i][j].m_parameters.threshold)
      && (g_object01_grid[i][j].m_parameters.alive == false))
      {
        grid_position.x = i;
        grid_position.y = j;
        agent_position = grid_to_agent_position(grid_position);
        add_object01_agent(agent_position, env);
      }
      //print matrix of intesity
      //std::cout << " "<< std::setprecision(1) << g_object01_grid[i][j].m_parameters.intensity;
    }
    //std::cout << std::endl;
  }
  //std::cout << std::endl;

  //prints active object01 grid positions if active_log_on == true
  if (ae::config::get["object01_track_agent"]["active_log_on"] == true)
  {
    int total_positions, val;
    val = ae::config::get["object01_track_agent"]["object01_grid_x"];
    total_positions = ae::config::get["object01_track_agent"]["object01_grid_y"];
    total_positions = total_positions * val;

    LOG(INFO) << "Active " << active_object01_value << "/" << total_positions << " Grid Pheromons";
  }
  //->
}

uint16_t Object01TrackAgent::assigned_type() const
{
  // plugin.cpp chcecks that this config entry exists
  return ae::config::get["agent_list"]["tracking"]["interface_type"];
}

//***testing position conversion
/*      //grid position
      sObject01GridPosition grid_position;
      //agent position
      ae::sAgentPosition agent_position = agent.position;
      std::cout << "1. x: " << agent.position.x << " y: " << agent.position.y << std::endl;
      grid_position = agent_to_grid_position(agent.position);
      std::cout << "2. x: " << grid_position.x << " y: " << grid_position.y << std::endl;
      agent_position = grid_to_agent_position(grid_position);
      std::cout << "3. x: " << agent_position.x << " y: " << agent_position.y << std::endl;
*/
//****
