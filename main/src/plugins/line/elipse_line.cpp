#include <cmath>

#include "common/agent.h"
#include "common/agent_body.h"
#include "common/config.h"
#include "common/timing.h"
#include "common/logging.h"

#include "elipse_line.h"


using namespace ae;
using nlohmann::json;


std::vector<Agent*> ElipseLine::spawn_line(const json &parameters)
{
  int agent_count = 400;
  float speed = 1.0;
  float radius = config::get["playground"]["size"][1].get<float>() * 0.31f;
  sColor color = { 1.0f, 0.0f, 0.0f};

  if (parameters.find("agent_count") != parameters.end() &&
      parameters["agent_count"].is_number_unsigned())
  {
    agent_count = parameters["agent_count"];
  }
  if (parameters.find("speed") != parameters.end() &&
      parameters["speed"].is_number())
  {
    speed = parameters["speed"];
  }
  if (parameters.find("radius") != parameters.end() &&
      parameters["radius"].is_number())
  {
    radius = parameters["radius"];
  }
  if (parameters.find("color") != parameters.end() &&
      parameters["color"].is_array() &&
      parameters["color"].size() == 3)
  {
    color.r = parameters["color"][0];
    color.g = parameters["color"][1];
    color.b = parameters["color"][2];
  }

  if(VLOG_IS_ON(1))
  {
    VLOG(1) << "Line option: agent_count=" << agent_count;
    VLOG(1) << "Line option: speed=" << speed;
    VLOG(1) << "Line option: radius=" << radius;
    VLOG(1) << "Line option: color=" << color.r << " " << color.g << " " << color.b;
  }

  std::vector<Agent*> agents;

  for (int i = 0; i < agent_count; i++)
  {
    ElipseLine *agent = new ElipseLine(i, agent_count, radius, speed, color);
    if (agent == nullptr)
    {
      LOG(ERROR) << "ElipseLine agent allocation error.";
    }
    else
    {
      agents.push_back(agent);
    }
  }

  return agents;
}


ElipseLine::ElipseLine(int index, int agent_count, float radius, float speed, const sColor &color) :
  Agent()
{
  // deforms circle in elipse to fit widescreen monitors
  const float aspect_ratio = 2.1;
  // position of this agent on circle circumference
  const float theta = 2.0f * M_PI * (float)index / (float)agent_count;

  m_initial_position.x = radius * aspect_ratio * cos(theta);
  m_initial_position.y = radius * sin(theta);
  m_speed = speed * 0.025;
  m_phase = 0;

  set_position();

  m_interface.color = color;
}


void ElipseLine::process(Environment &env)
{
  (void)env;

  m_interface.timestamp = time::timestamp();
  m_interface.expires = time::future_timestamp(time::seconds(2));

  m_phase += m_speed;
  set_position();
}


uint16_t ElipseLine::assigned_type() const
{
  return config::get["agent_list"]["line"]["interface_type"];
}

void ElipseLine::set_position()
{
  m_interface.position.x = m_initial_position.x;
  // adds the wobly effect on y axis
  m_interface.position.y = m_initial_position.y + sin(m_phase + m_initial_position.x / 5.0f);
}
