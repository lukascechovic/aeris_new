#include <vector>

#include "common/agent.h"  // pre ae::Agent
#include "common/logging.h"
#include "common/plugin.h" // pre ae::plugin::plugin_t, ae::plugin::plugin_init, nlohmann::json, el::base::type::StoragePointer

#include "tracking.h"


// mená, ktoré používame
using json = nlohmann::json;
using el::base::type::StoragePointer;
using namespace ae;
using ae::plugin::plugin_t;
using ae::plugin::plugin_init_t;
using ae::plugin::plugin_init;


// forward declarations for plugin functions
extern "C" plugin_t* tracking_load();
int tracking_init(plugin_init_t &init_data);
std::vector<Agent*> tracking_create(const json &parameters);


// štruktúra pluginu
static plugin_t tracking_plugin = {
  tracking_init, // or just ae::plugin::plugin_init
  tracking_create
};


// plugin load function
extern "C" plugin_t* tracking_load()
{
  return &tracking_plugin;
}


// Inicializuje plugin a vypíše správu do logu.
int tracking_init(plugin_init_t &init_data)
{
  // nastav globálne premenné
  plugin_init(init_data);

  // tu si plugin môže robiť svoje veci

  // napr môže skontrolovať, že konfiguračný súbor naozaj obsahuje položky pre
  // agentov ktorých tento plugin vytvára
  const auto &agent_list = config::get["agent_list"];
  if (agent_list.find("tracking") == agent_list.end())
  {
    LOG(ERROR) << "Configuration is missing 'agent_list' entry for 'tracking'";
    return -1;
  }

  LOG(INFO) << "TrackingAgent plugin initialized.";

  // vráť nulu pri úspešnej inicializácii
  return 0;
}


// Funkcia, ktorá vytvára agentov na základe predaných parametrov.
std::vector<Agent*> tracking_create(const json &parameters)
{
  std::vector<Agent*> arr;

  TrackingAgent *agent = new TrackingAgent(parameters);
  if (agent != nullptr)
  {
    arr.push_back(agent);
    //env.add_agent(agent); //vyuzit pre feromon na pridavanie
    //nedeletovat spravovane env
  }

  return arr;
}
