#ifndef ARIA_COMMON_LOGGING_H_
#define ARIA_COMMON_LOGGING_H_

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

#define CHECK(expr)                                            \
  if (!(expr)) {                                               \
    spdlog::critical("Check failed: {} at {}:{}, function {}", \
                     #expr,                                    \
                     __FILE__,                                 \
                     __LINE__,                                 \
                     __PRETTY_FUNCTION__);                     \
    std::abort();                                              \
  }

inline void LOG_DATA(std::string key, bool success, std::string msg) {
  auto data_logger = spdlog::get("data_logger");
  key += success ? "_success" : "";
  data_logger->info(HL(key) + " " + msg);
}

inline std::string printKeyPoints(std::vector<cv::KeyPoint> keypoints) {
  std::stringstream ss;
  for (const auto& kp : keypoints) {
    ss << kp.pt << " ";
  }
  return ss.str();
}

namespace aria::logging {

void initialize_logger(std::filesystem::path log_dir, std::string name);

std::filesystem::path initializeOutputsDirectory(const std::string& output_dir,
                                                 const std::string& tag);

}  // namespace aria::logging

#endif  // ARIA_COMMON_LOGGING_H_