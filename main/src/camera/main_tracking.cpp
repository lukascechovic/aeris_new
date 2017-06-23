#include <iostream>
#include <memory>
#include <exception>
#include <stdexcept>

#include "common/config.h"
#include "common/logging.h"

#include "robot_tracking.h"


int main(int argc, char *argv[])
{
  using tracking_ptr = std::unique_ptr<RobotTracking>;
  tracking_ptr tracker;

  try
  {
    if (ae::config::setup("robot_tracking", argc, argv) < 0)
    {
      return 1;
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << "Failed to initialize Aeris: " << e.what();
    return 1;
  }

  try
  {
    const auto &config = ae::config::get;
    if (config.find("robot_tracking") == config.end())
    {
      tracker = tracking_ptr(new RobotTracking());
    }
    else
    {
      const auto &tracker_config = config["robot_tracking"];
      if (tracker_config.is_object())
      {
        tracker = tracking_ptr(new RobotTracking(tracker_config));
      }
      else
      {
        LOG(ERROR) << "Invalid setting value for \'" << "robot_tracking"
                   << "\'. Expected " << "object"
                   << ". Got: " << tracker_config;
        throw std::runtime_error("Configuration parsing error.");
      }
    }
  }
  catch (const std::exception &e)
  {
    LOG(ERROR) << "Failed to initialize RobotTracking: " << e.what();
    return 1;
  }


  tracker->run();

  return 0;
}
