#include <cstdint>

#include <memory>
#include <mutex>
#include <vector>
#include <thread>


#include "common/agent.h"
#include "common/config.h"
#include "common/timing.h"
#include "common/logging.h"

#include "visualisation_agent.h"

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>


const std::string VisualisationAgent::kAgentName = "visualisation";
const std::string VisualisationAgent::kDefaultWindowName = "robot visualisation";


using namespace ae;


VisualisationAgent::VisualisationAgent(const nlohmann::json &parameters) :
  m_fullscreen(kDefaultFullscreen),
  m_draw_grid(kDefaultDrawGrid),
  m_resolution({kDefaultResolutionX, kDefaultResolutionY}),
  m_framerate(kDefaultFrameRate),
  m_window_name(kDefaultWindowName),

  m_buffer(nullptr),
  m_lock_buffer(),

  m_window_handle(-1),
  m_running(false),
  m_glut_thread(),
  m_camera_offset(0),
  m_cm_size(0)
{
  // initialize agent interface
  m_interface.type = this->assigned_type();
  m_interface.body = AgentBody::get_body_type(m_interface.type);
  m_interface.timestamp = time::timestamp();
  m_interface.expires = time::future_timestamp(time::seconds(10));
  m_interface.position.x = 0;
  m_interface.position.y = 0;
  m_interface.position.z = 0;
  m_interface.color.r = 0;
  m_interface.color.g = 0;
  m_interface.color.b = 0;

  // load options from parameters
  if (!parameters.is_object())
  {
    LOG(ERROR) << "Invalid arguments for visualisation agent: " << parameters;
  }
  else
  {
    if (parameters.find("fullscreen") != parameters.end() &&
        parameters["fullscreen"].is_boolean())
    {
      m_fullscreen = parameters["fullscreen"];
    }
    if (parameters.find("draw_grid") != parameters.end() &&
        parameters["draw_grid"].is_boolean())
    {
      m_draw_grid = parameters["draw_grid"];
    }
    if (parameters.find("resolution") != parameters.end() &&
        parameters["resolution"].is_array() &&
        parameters["resolution"].size() == 2)
    {
      m_resolution.x = parameters["resolution"][0];
      m_resolution.y = parameters["resolution"][1];
    }
    if (parameters.find("framerate") != parameters.end() &&
        parameters["framerate"].is_number_unsigned())
    {
      m_framerate = parameters["framerate"];
    }
    if (parameters.find("window_name") != parameters.end() &&
        parameters["window_name"].is_string())
    {
      m_window_name = parameters["window_name"];
    }
  }


  m_camera_offset = (config::get["playground"]["size"][1].get<double>() / 2) / tan(45.0 / 360.0 * 3.14);
  m_cm_size = m_resolution.x / config::get["playground"]["size"][0].get<double>();

  if (VLOG_IS_ON(4))
  {
    VLOG(4) << "Visualisation option: " << "fullscreen=" << m_fullscreen;
    VLOG(4) << "Visualisation option: " << "draw_grid=" << m_draw_grid;
    VLOG(4) << "Visualisation option: " << "resolution.x=" << m_resolution.x;
    VLOG(4) << "Visualisation option: " << "resolution.y=" << m_resolution.y;
    VLOG(4) << "Visualisation option: " << "framerate=" << m_framerate << "fps";
    VLOG(4) << "Visualisation option: " << "window_name=" << m_window_name;
    VLOG(4) << "Visualisation setting: " << "camera_offset=" << m_camera_offset << "cm";
    VLOG(4) << "Visualisation setting: " << "cm_size=" << m_cm_size << "px";
  }

  m_glut_thread = std::thread(&VisualisationAgent::glut_thread_callback, this);
}


VisualisationAgent::~VisualisationAgent()
{
  m_running = false;

  if (m_glut_thread.joinable())
  {
    m_glut_thread.join();
  }
}


void VisualisationAgent::process(Environment &env)
{
  agent_buffer_t new_data = agent_buffer_t(new std::vector<sAgentInterface>(env.global_state()));

  m_lock_buffer.lock();
  m_buffer = new_data;
  m_lock_buffer.unlock();

  m_interface.timestamp = time::timestamp();
  m_interface.expires = time::future_timestamp(time::seconds(10));
}


uint16_t VisualisationAgent::assigned_type() const
{
  return config::get["agent_list"][kAgentName]["interface_type"];
}


void VisualisationAgent::glut_thread_callback()
{
  {
    int argc = 0;
    glutInit(&argc, NULL);
  }
  glutInitWindowSize(m_resolution.x, m_resolution.y);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  m_window_handle = glutCreateWindow(m_window_name.c_str());

  glViewport(0, 0, m_resolution.x, m_resolution.y);
  glMatrixMode(GL_PROJECTION);
  glEnable(GL_DEPTH_TEST);
  gluPerspective(45, (float) m_resolution.x / m_resolution.y, 0.1, 2000);
  glMatrixMode(GL_MODELVIEW);

  if (m_fullscreen)
  {
    glutFullScreen();
  }

  auto frame_period = ae::time::microseconds((int)(1.0 / (double)m_framerate * 1000000.0));
  auto next_frame = ae::time::clock::now();

  make_grid();

  m_running = true;
  while (m_running)
  {
    draw_scene();

    next_frame += frame_period;
    ae::time::sleep_until(next_frame);
  }

  if (m_window_handle != -1)
  {
    glutDestroyWindow(m_window_handle);
  }
}


void VisualisationAgent::draw_scene()
{
  TIMED_FUNC(draw_scene_timer);

  m_lock_buffer.lock();
  agent_buffer_t agents = m_buffer;
  m_lock_buffer.unlock();

  // set viewport
  glMatrixMode(GL_PROJECTION);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_MODELVIEW);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glMatrixMode(GL_MODELVIEW);

  glTranslatef(0.0f, 0.0f, -m_camera_offset);
  glRotatef(0.0f, 0.0f, 0.0f, 0.0f);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // draw grid
  draw_grid();

  // draw agents
  if (agents && agents->size() > 0)
  {
    for (const sAgentInterface &agent : *agents)
    {
      draw_agent(agent);
    }
  }

  glutSwapBuffers();
}


void VisualisationAgent::draw_grid()
{
  unsigned int max_y = 100;
  unsigned int max_x = (max_y*16.0)/9.0;
  unsigned int j, i;

  for (j = 0; j < max_y; j++)
  {
    for (i = 0; i < max_x; i++)
    {
      if ((j%2) == (i%2))
        glColor3f(0.05, 0.05, 0.05);
      else
        glColor3f(0.0, 0.0, 0.0);

      float size = 1; //cm_size;
      float x = size*(i - max_x/2.0);
      float y = size*(j - max_y/2.0);


      glBegin(GL_QUADS);
      glVertex3f(x - 0.5*size, y - 0.5*size, 0.0);
      glVertex3f(x + 0.5*size, y - 0.5*size, 0.0);
      glVertex3f(x + 0.5*size, y + 0.5*size, 0.0);
      glVertex3f(x - 0.5*size, y + 0.5*size, 0.0);
      glEnd();
    }
  }
}


void VisualisationAgent::draw_agent(const sAgentInterface &agent)
{
    // float cm_size = g_configure.get_cm_size();
    //
    // float x_ofs = agent.position.x * cm_size;
    // float y_ofs = agent.position.y * cm_size;
    // float z_ofs = agent.position.z * cm_size;

    glPushMatrix();

    glTranslatef(agent.position.x, agent.position.y, agent.position.z);
    glRotatef(agent.position.roll, 0.0f, 1.0f, 0.0f);
    glRotatef(agent.position.pitch, 1.0f, 0.0f, 0.0f);
    glRotatef(agent.position.yaw, 0.0f, 0.0f, 1.0f);

    const AgentBody *body = AgentBody::get_body(agent.body);
    if (body->vertices().size() > 0)
    {
      glColor3f(agent.color.r, agent.color.g, agent.color.b);

      glBegin(GL_TRIANGLES);
      for (const auto &vert : body->vertices())
      {
        glVertex3f(vert.x, vert.y, vert.z);
      }
      glEnd();
    }

    glPopMatrix();
}


void VisualisationAgent::make_grid()
{
  int count_x = config::get["playground"]["size"][0].get<int>() + 1;
  if (count_x & 1)
  {
    count_x += 1;
  }
  int count_y = config::get["playground"]["size"][1].get<int>() + 1;
  if (count_y & 1)
  {
    count_y += 1;
  }
  count_x /= 2;
  count_y /= 2;

  std::vector<ae::Point3D> line;
  for (float x = -count_x; x < count_x; x += 2)
  {
    line.push_back({x,        0.0f, 0.0f});
    line.push_back({x + 1.0f, 0.0f, 0.0f});
    line.push_back({x,        1.0f, 0.0f});
    line.push_back({x + 1.0f, 0.0f, 0.0f});
    line.push_back({x + 1.0f, 1.0f, 0.0f});
    line.push_back({x,        1.0f, 0.0f});
  }

  m_grid = line;
}
