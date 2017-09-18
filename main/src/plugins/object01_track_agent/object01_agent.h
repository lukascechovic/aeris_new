#ifndef _OBJECT_01_AGENT_H_
#define _OBJECT_01_AGENT_H_

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

class Object01Agent : public ae::Agent
{
  private:
    grid_postion m_grid_position;
  public:
    Object01Agent();
    Object01Agent(float x, float y);
    Object01Agent(ae::sAgentPosition position);
    ~Object01Agent() {}


    void process(ae::Environment &env);

    uint16_t assigned_type() const;

    bool remove_protection() const { return false; }
};

struct sObject01GridPosition
{
  uint64_t x;
  uint64_t y;
};

struct sObject01Params
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

class Object01GridSize
{
  protected:


  public:
    Object01GridSize();
    ~Object01GridSize() {}
    sGridSize m_gridsize;
    uint64_t x;
    uint64_t y;
};

//Object01GridSize Object01 grid;

class Object01Params //: public Object01 grid
{
  protected:

  private:

  public:
    Object01Params();
    ~Object01Params() {}
    sObject01Params m_parameters;
};

#endif /* _OBJECT_01_AGENT_H_ */
