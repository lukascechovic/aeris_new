#ifndef _EXAMPLE_AGENT_H_
#define _EXAMPLE_AGENT_H_

#include <cstdint>

#include <memory>
#include <mutex>
#include <vector>
#include <thread>

#include "common/agent.h"
#include "common/agent_body.h"
#include "common/config.h"


class VisualisationAgent : public ae::Agent
{
  public:
    static const std::string kAgentName;

    static const bool kDefaultFullscreen = true; /* true/false */
    static const bool kDefaultDrawGrid = true; /* true/false */
    static const int kDefaultResolutionX = 1920; /* pixels */
    static const int kDefaultResolutionY = 1080; /* pixels */
    static const int kDefaultFrameRate = 30; /* frames per second */
    static const std::string kDefaultWindowName; /* "robot visualisation" */

  private:
    using agent_buffer_t = std::shared_ptr<std::vector<ae::sAgentInterface>>;


  private:

    bool m_fullscreen;
    bool m_draw_grid;
    struct { int x, y; } m_resolution;
    int m_framerate;
    std::string m_window_name;


    agent_buffer_t m_buffer;
    std::mutex m_lock_buffer;

    int m_window_handle;
    bool m_running;
    std::thread m_glut_thread;


    /** \brief How far to move camera from center to contain whole playground */
    double m_camera_offset;

    /** \brief How many pixels per cm there are.
     *
     * Derived from window resolution and playground size.
     */
    double m_cm_size;

    /** \brief Vertices of background grid. */
    std::vector<ae::Point3D> m_grid;

    /** \brief Vertices of border. */
    std::vector<ae::Point3D> m_border;

  public:
    VisualisationAgent(const nlohmann::json &parameters);
    ~VisualisationAgent();

    /** \brief Makes a copy of global state that is later used for rendering */
    void process(ae::Environment &env);

    uint16_t assigned_type() const;

    bool remove_protection() const { return false; }

    bool is_running()
    {
      return m_running;
    }

  private:
    /** \brief Main render loop.
     *
     * GLUT and OpenGL are not thread safe so all calls to their apis have to be
     * from this thread only.
     */
    void glut_thread_callback();

    void draw_scene();
    void draw_grid();
    void draw_triangles(const std::vector<ae::Point3D> &points);
    void draw_agent(const ae::sAgentInterface &agent);

    void make_grid(); /* TODO */
    void make_border(const float width);

    public:
      void rendering();
      void mouse_move_handler(int x, int y);
      void touch_motion_handler(int x, int y);
};


#endif /* _EXAMPLE_AGENT_H_ */
