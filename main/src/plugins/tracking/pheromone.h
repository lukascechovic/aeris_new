#ifndef _PHEROMONE_AGENT_H_
#define _PHEROMONE_AGENT_H_

#include <cstdint> // for integer types

#include "common/agent.h" // for ae::Agent, ae::Environment
#include "common/config.h" // for nlohmann::json

//CREATE ERRORS
//#include "tracking.h"

struct grid_postion
{
  uint64_t x;
  uint64_t y;
};

class PheromoneAgent : public ae::Agent
{
  private:
    grid_postion m_grid_position;
  public:
    PheromoneAgent();
    PheromoneAgent(float x, float y);
    PheromoneAgent(ae::sAgentPosition position);
    ~PheromoneAgent() {}


    void process(ae::Environment &env);

    uint16_t assigned_type() const;

    bool remove_protection() const { return false; }
};

struct sPheromoneGridPosition
{
  uint64_t x;
  uint64_t y;
};

struct sPheromoneParams
{
  uint64_t id;
  float intensity;
  float fade_speed;
  float rise_speed;
  float rise_from;
  bool alive;
  float threshold;
};

struct sGridSize
{
  uint64_t x;
  uint64_t y;
};

class PheromoneGridSize
{
  protected:


  public:
    PheromoneGridSize();
    ~PheromoneGridSize() {}
    sGridSize m_gridsize;
    uint64_t x;
    uint64_t y;
};

//PheromoneGridSize pheromonegrid;

class PheromoneParams //: public pheromonegrid
{
  protected:

  private:

  public:
    PheromoneParams();
    ~PheromoneParams() {}
    sPheromoneParams m_parameters;
};

#endif /* _PHEROMONE_AGENT_H_ */
