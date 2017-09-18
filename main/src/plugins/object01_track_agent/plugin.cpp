#include <vector>

#include "common/agent.h"  // pre ae::Agent
#include "common/logging.h"
#include "common/plugin.h" // pre ae::plugin::plugin_t, ae::plugin::plugin_init, nlohmann::json, el::base::type::StoragePointer

#include "object01_track_agent.h"


// mená, ktoré používame
using json = nlohmann::json;
using el::base::type::StoragePointer;
using namespace ae;
using ae::plugin::plugin_t;
using ae::plugin::plugin_init_t;
using ae::plugin::plugin_init;


// forward declarations for plugin functions
extern "C" plugin_t* object01_track_agent_load();
int object01_track_agent_init(plugin_init_t &init_data);
std::vector<Agent*> object01_track_agent_create(const json &parameters);


// štruktúra pluginu
static plugin_t object01_track_agent_plugin = {
  object01_track_agent_init, // or just ae::plugin::plugin_init
  object01_track_agent_create
};


// plugin load function
extern "C" plugin_t* object01_track_agent_load()
{
  return &object01_track_agent_plugin;
}


// Inicializuje plugin a vypíše správu do logu.
int object01_track_agent_init(plugin_init_t &init_data)
{
  // nastav globálne premenné
  plugin_init(init_data);

  // tu si plugin môže robiť svoje veci

  // napr môže skontrolovať, že konfiguračný súbor naozaj obsahuje položky pre
  // agentov ktorých tento plugin vytvára
  const auto &agent_list = config::get["agent_list"];
  if (agent_list.find("object01_track_agent") == agent_list.end())
  {
    LOG(ERROR) << "Configuration is missing 'agent_list' entry for 'object01_track_agent'";
    return -1;
  }

  LOG(INFO) << "Object01TrackAgent plugin initialized.";

  // vráť nulu pri úspešnej inicializácii
  return 0;
}


// Funkcia, ktorá vytvára agentov na základe predaných parametrov.
std::vector<Agent*> object01_track_agent_create(const json &parameters)
{
  std::vector<Agent*> arr;

  Object01TrackAgent *agent = new Object01TrackAgent(parameters);
  if (agent != nullptr)
  {
    arr.push_back(agent);
    //env.add_agent(agent); //vyuzit pre feromon na pridavanie
    //nedeletovat spravovane env
  }

  return arr;
}
