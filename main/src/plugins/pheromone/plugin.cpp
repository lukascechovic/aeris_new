#include <vector>

#include "common/agent.h"  // pre ae::Agent
#include "common/logging.h"
#include "common/plugin.h" // pre ae::plugin::plugin_t, ae::plugin::plugin_init, nlohmann::json, el::base::type::StoragePointer

#include "pheromone.h"


// mená, ktoré používame
using json = nlohmann::json;
using el::base::type::StoragePointer;
using namespace ae;
using ae::plugin::plugin_t;
using ae::plugin::plugin_init_t;
using ae::plugin::plugin_init;


// forward declarations for plugin functions
extern "C" plugin_t* Pheromone_load();
int Pheromone_init(plugin_init_t &init_data);
std::vector<Agent*> Pheromone_create(const json &parameters);


// štruktúra pluginu
static plugin_t Pheromone_plugin = {
  Pheromone_init, // or just ae::plugin::plugin_init
  Pheromone_create
};


// plugin load function
extern "C" plugin_t* Pheromone_load()
{
  return &Pheromone_plugin;
}


// Inicializuje plugin a vypíše správu do logu.
int Pheromone_init(plugin_init_t &init_data)
{
  // nastav globálne premenné
  plugin_init(init_data);

  // tu si plugin môže robiť svoje veci

  // napr môže skontrolovať, že konfiguračný súbor naozaj obsahuje položky pre
  // agentov ktorých tento plugin vytvára
  const auto &agent_list = config::get["agent_list"];
  if (agent_list.find("Pheromone") == agent_list.end())
  {
    LOG(ERROR) << "Configuration is missing 'agent_list' entry for 'Pheromone'";
    return -1;
  }

  LOG(INFO) << "PheromoneAgent plugin initialized.";

  // vráť nulu pri úspešnej inicializácii
  return 0;
}


// Funkcia, ktorá vytvára agentov na základe predaných parametrov.
std::vector<Agent*> Pheromone_create(const json &parameters)
{
  std::vector<Agent*> arr;

  PheromoneAgent *agent = new PheromoneAgent(parameters);
  if (agent != nullptr)
  {
    arr.push_back(agent);
    //env.add_agent(agent); //vyuzit pre feromon na pridavanie
    //nedeletovat spravovane env
  }

  return arr;
}
