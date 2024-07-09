#ifndef ARIA_COMMON_LOGGING_H_
#define ARIA_COMMON_LOGGING_H_

#include <fmt/core.h>
#include <g2o/types/sim3/sim3.h>
#include <gtsam/geometry/Pose3.h>
#include <gtsam/slam/BetweenFactor.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <opencv2/core.hpp>
#include <string>

#define HL(msg) (std::string("<: ") + (msg) + std::string(" /:>").c_str())

#define LOG_FATAL(msg)                                      \
  spdlog::critical("Fatal error: {} at {}:{}, function {}", \
                   msg,                                     \
                   __FILE__,                                \
                   __LINE__,                                \
                   __PRETTY_FUNCTION__);                    \
  std::abort();

// cancel glog's CHECK macro
#ifdef CHECK
#undef CHECK
#endif

#define CHECK_LT(a, b) CHECK((a) < (b))

#define CHECK_NE(a, b) CHECK((a) != (b))

#define CHECK_EQ(a, b) CHECK((a) == (b))

#define CHECK_GE(a, b) CHECK((a) >= (b))

#define CHECK_GT(a, b) CHECK((a) > (b))

#define CHECK_MSG(expr, msg)                                       \
  if (!(expr)) {                                                   \
    spdlog::critical("Check failed: {} at {}:{}, function {}, {}", \
                     #expr,                                        \
                     __FILE__,                                     \
                     __LINE__,                                     \
                     __PRETTY_FUNCTION__,                          \
                     msg);                                         \
    std::abort();                                                  \
  }

#define CHECK(expr) CHECK_MSG(expr, "")

using namespace gtsam;

inline void LOG_DATA(std::string key, bool success, std::string msg) {
  auto data_logger = spdlog::get("data_logger");
  key += success ? "_success" : "";
  data_logger->info(HL(key) + " " + msg);
  // also log to default logger
  spdlog::info(HL(key) + " " + msg);
}

inline std::string printKeyPoints(std::vector<cv::KeyPoint> keypoints,
                                  std::vector<bool> mask) {
  std::stringstream ss;
  for (size_t i = 0; i < keypoints.size(); i++) {
    if (mask[i]) {
      ss << keypoints[i].pt << " ";
    }
  }
  return ss.str();
}

inline std::string printKeyPointMatches(std::vector<cv::KeyPoint> keypoints0,
                                        std::vector<cv::KeyPoint> keypoints1,
                                        std::string key = "",
                                        std::vector<bool> mask = {}) {
  CHECK(keypoints0.size() == keypoints1.size());

  if (mask.size()) {
    CHECK(mask.size() == keypoints0.size())
  } else {
    mask = std::vector<bool>(keypoints0.size(), true);
  }

  std::stringstream ss;

  if (key.size()) ss << key << " ";
  for (size_t i = 0; i < keypoints0.size(); i++) {
    if (mask[i]) {
      ss << keypoints0[i].pt << " " << keypoints1[i].pt << " ";
    }
  }

  return ss.str();
}

inline std::string printSim3(g2o::Sim3 gSel) {
  // quaternion to euler
  Eigen::Vector3d euler =
      gSel.rotation().toRotationMatrix().eulerAngles(0, 1, 2);

  // Using fmt::format to format the string with object method calls
  std::string formatted_string = fmt::format(
      "{:.1f}, {:.1f}, {:.1f}, {:.1f}, {:.1f}, {:.1f}, "
      "{:.1f}",
      euler.x(),
      euler.y(),
      euler.z(),
      gSel.translation().x(),
      gSel.translation().y(),
      gSel.translation().z(),
      gSel.scale());

  return formatted_string;
}

template <class VALUE>
inline std::string printValueG2o(const Key& key, const VALUE& value) = delete;

template <>
inline std::string printValueG2o(const Key& key, const Pose3& value) {
  std::stringstream ss;
  ss << "VERTEX_SE3:QUAT " << key << " " << value.x() << " " << value.y() << " "
     << value.z() << " " << value.rotation().toQuaternion().x() << " "
     << value.rotation().toQuaternion().y() << " "
     << value.rotation().toQuaternion().z() << " "
     << value.rotation().toQuaternion().w();

  return ss.str();
}

template <class VALUE>
inline std::string printFactorG2o(const VALUE& factor) = delete;

template <>
inline std::string printFactorG2o(const BetweenFactor<Pose3>& factor) {
  std::stringstream ss;
  ss << "EDGE_SE3:QUAT " << factor.key1() << " " << factor.key2() << " "
     << factor.measured().x() << " " << factor.measured().y() << " "
     << factor.measured().z() << " "
     << factor.measured().rotation().toQuaternion().x() << " "
     << factor.measured().rotation().toQuaternion().y() << " "
     << factor.measured().rotation().toQuaternion().z() << " "
     << factor.measured().rotation().toQuaternion().w() << " "
     << factor.noiseModel()->sigmas().transpose();

  return ss.str();
}

namespace aria::logging {

using namespace gtsam;

void initializeLogger(std::filesystem::path log_dir, std::string name);

std::filesystem::path initializeOutputsDirectory(const std::string& output_dir,
                                                 const std::string& tag);

}  // namespace aria::logging

#endif  // ARIA_COMMON_LOGGING_H_