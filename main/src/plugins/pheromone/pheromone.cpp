#include "common/agent.h"  // pre ae::Agent, ae::Environment
#include "common/agent_body.h"  // pre ae::AgentBody
#include "common/config.h"  // pre ae::config::get
#include "common/timing.h" // pre ae::time::sleep_for, ae::time::milliseconds
#include "common/logging.h" // pre LOG()

#include "pheromone.h"


PheromoneAgent::PheromoneAgent(const nlohmann::json &parameters) :
  Agent()
{
  (void) parameters;
  m_interface.position.x = (rand() % 20) - 10;
  m_interface.position.y = (rand() % 20) - 10;

  m_interface.color = {0.0f, 0.5f, 0.5f};
  m_interface.type = 200;
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

/** \brief Napíše do logu, že  bola zavolaná táto funkcia a čaká 5 ms. */
void PheromoneAgent::process(ae::Environment &env)
{
  (void)env;



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
    if (agent.type == target_type)
    {
      //zistim jeho poziciu
      //
      //vytvorim noveho agenta noveho typu feromon
      //
      const uint16_t target_type_pheromone = ae::config::get["agent_list"]["pheromone"]["interface_type"];
      (void) target_type_pheromone;
      ae::Agent *agent_to_add;
      (void) agent_to_add;
      //ae::Agent *agent_to_add = new PheromoneAgent;
      //nastavenie parametrov agenta pre prianie
      //
      //agent_to_add.m_interface.type = target_type_pheromone;
      //agent_to_add.position.x = m_interface.position.x;
      //agent_to_add.position.y = m_interface.position.y;
      //agent_to_add.color = {0.0f, 1.0f, 1.0f};
      //
      //pridanie agenta do prostredia
      //env.add_agent (agent_to_add);
    }
  }

}


uint16_t PheromoneAgent::assigned_type() const
{
  // plugin.cpp chcecks that this config entry exists
  return ae::config::get["agent_list"]["pheromone"]["interface_type"];
}
