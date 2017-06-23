#ifndef _SEGMENTATION_H_
#define _SEGMENTATION_H_

#include <cstdint>

#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/video/background_segm.hpp>

#include "common/config.h"
#include "common/logging.h"


/** \addtogroup robot-tracking
 *  @{
 */


class Segmentation
{
public:
  static const uint32_t kMinKeypoints = 100;
  static const uint32_t kMinCentroids = 5;
  static const uint32_t kRequiredOverlap = 3;
  static const int kMaxFastThreshold = 200;
  static const int kMaxResponseThreshold = 4000;

  static const uint32_t kDefaultRobotSize = 26;
  static const uint32_t kDefaultFastThreshold = 70;
  static const int32_t kDefaultResponseThreshold = 1000;
  static const bool kDefaultBackgroundSubstraction = false;

public:
  Segmentation(cv::Size input_size);

  std::vector<cv::Point> compute(const cv::Mat &frame);

  const cv::Mat& get_mask() const { return m_mask; }
  const cv::Mat& get_playground_mask() const { return m_playground_mask; }
  const std::vector<cv::KeyPoint>& getKeypoints() const { return m_keypoints; }
  const std::vector<cv::Point3f>& getWeightedCentroids() const { return m_centroids; }
  uint32_t getFastThreshold() const { return m_fast_threshold; }
  float getResponseThreshold() const { return m_response_threshold; }

  void setPlaygroundCorners(const std::vector<cv::Point> &corners);
  void setRobotSize(uint32_t value) { m_robot_size = value; }
  void setFastThreshold(int value) { m_fast_threshold = value; }
  void setResponseThreshold(int value) { m_response_threshold = value; }
  void setBackgroundSubstraction(bool value) { m_background_sub = value; }

  void loadConfiguration(const nlohmann::json &settings);
  void createFastTrackbar(const std::string &window_name);
  void createResponseTrackbar(const std::string &window_name);

private:
  std::vector<cv::KeyPoint> computeKeypoints(const cv::Mat &frame);
  std::vector<cv::Point> findCentroids(const std::vector<cv::Point3f> &points);

private:
  cv::Size m_input_size;
  cv::Mat m_playground_mask;
  uint32_t m_robot_size;
  int m_fast_threshold;
  int m_response_threshold;
  bool m_background_sub;

  cv::Mat m_mask;
  std::vector<cv::KeyPoint> m_keypoints;

  cv::Ptr<cv::BackgroundSubtractor> m_bgsub;
  cv::Ptr<cv::FastFeatureDetector> m_features;
  uint32_t m_centroid_count;
  std::vector<cv::Point3f> m_centroids;
};


inline MAKE_LOGGABLE(cv::KeyPoint, kp, os)
{
  os << "([" << kp.pt.x << ", " << kp.pt.y << "], "<< kp.response << ")";
  return os;
}


/** @} */


#endif /* _SEGMENTATION_H_ */
