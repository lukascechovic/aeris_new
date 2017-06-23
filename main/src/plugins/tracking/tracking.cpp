#include "common/agent.h"  // pre ae::Agent, ae::Environment
#include "common/agent_body.h"  // pre ae::AgentBody
#include "common/config.h"  // pre ae::config::get
#include "common/timing.h" // pre ae::time::sleep_for, ae::time::milliseconds
#include "common/logging.h" // pre LOG()

#include "tracking.h"
#include "pheromone.h"


TrackingAgent::TrackingAgent(const nlohmann::json &parameters) :
  Agent()
{
  (void) parameters;
  m_interface.position.x = 0;//(rand() % 20) - 10;
  m_interface.position.y = 0;//(rand() % 20) - 10;

  m_interface.color = {0.0f, 1.0f, 1.0f};
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
 * [TrackingAgent::add_pheromone]
 * @param position [position of pheromone to create]
 * @param &env [environment to add pheromone on]
 */

void TrackingAgent::add_pheromone (ae::sAgentPosition position, ae::Environment &env)
{
  PheromoneAgent *agent_to_add = new PheromoneAgent(position);
  //set position to tracked agent

  if (agent_to_add == nullptr)
  {
    LOG(ERROR) << "PheromoneAgent allocation error.";
  }
  else
  {
    //agents.push_back(agent);
    LOG(INFO) << "PheromoneAgent Created. Number: ";// << agent_to_add.id;
    env.add_agent(agent_to_add);
  }
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

      add_pheromone(agent.position, env);

        //TODO HANDLE AND KILL PHEROMONES
        //TODO checking position
        //TODO hold all active positions with intensity
        //TODO maybe pheromone matrix
        //TODO cycle to check active points and write them
    }
  }
  //TODO basic pheromone grid structure
  int pheromone_grid_x = 36;
  int pheromone_grid_y = 20;
  std::vector<std::vector<float>> pheromone_grid;
  pheromone_grid.resize(pheromone_grid_x);

  for (size_t i = 0; i < pheromone_grid.size(); i++)
  {
    pheromone_grid[i].resize(pheromone_grid_y);
  }
  for (size_t i = 0; i < pheromone_grid.size(); i++)
  {
    for (size_t j = 0; j < pheromone_grid[i].size(); j++)
    {
      pheromone_grid[i][j] = 0;
    }
  }

}


uint16_t TrackingAgent::assigned_type() const
{
  // plugin.cpp chcecks that this config entry exists
  return ae::config::get["agent_list"]["tracking"]["interface_type"];
}
