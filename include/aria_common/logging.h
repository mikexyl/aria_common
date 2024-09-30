#ifndef ARIA_COMMON_LOGGING_H_
#define ARIA_COMMON_LOGGING_H_

#include <fmt/format.h>
#include <gtsam/geometry/Pose3.h>
#include <gtsam/slam/BetweenFactor.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <fstream>
#include <opencv2/core.hpp>
#include <string>

#ifdef USE_G2O
#include <g2o/types/sim3/sim3.h>
#endif

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

namespace aria::logging {
extern std::string dataFile;
extern YAML::Node dataRoot;
extern int FlashDataEveryN;
extern int FlashDataCounter;
void flashData();
}  // namespace aria::logging

template <typename T>
inline void LOG_DATA(std::string key, T msg) {
  // convert msg to string
  std::stringstream ss;
  ss << msg;
  auto msg_str = ss.str();
  LOG_DATA(key, msg_str);
}

template <>
inline void LOG_DATA(std::string key, std::string msg) {
  spdlog::info(HL(key) + " " + msg);

  // Check if the key already exists in dataRoot
  if (!aria::logging::dataRoot[key] ||
      !aria::logging::dataRoot[key].IsSequence()) {
    // If the key doesn't exist or is not a sequence, initialize it as a
    // sequence
    aria::logging::dataRoot[key] = YAML::Node(YAML::NodeType::Sequence);
  }

  // Append the message to the sequence
  aria::logging::dataRoot[key].push_back(msg);

  // Increment the counter
  aria::logging::FlashDataCounter++;
  if (aria::logging::FlashDataCounter >= aria::logging::FlashDataEveryN) {
    // Write the data to the file
    aria::logging::flashData();
  }
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

#ifdef USE_G2O
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
#endif

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

class SpdlogBuf : public std::streambuf {
 public:
  SpdlogBuf() = default;

  static SpdlogBuf& instance() {
    static SpdlogBuf buf;
    return buf;
  }

 protected:
  int overflow(int c) override {
    if (c != EOF) {
      buffer_ += static_cast<char>(c);
      if (c == '\n') {
        logBuffer();
      }
    }
    return c;
  }

  std::streamsize xsputn(const char* s, std::streamsize n) override {
    buffer_.append(s, n);
    std::string::size_type pos = 0;
    while ((pos = buffer_.find('\n')) != std::string::npos) {
      logBuffer(pos + 1);
    }
    return n;
  }

 private:
  void logBuffer(std::string::size_type length = std::string::npos) {
    if (length == std::string::npos) {
      spdlog::info(buffer_);
      buffer_.clear();
    } else {
      spdlog::info(buffer_.substr(0, length));
      buffer_.erase(0, length);
    }
  }

  std::string buffer_;
};

void initializeLogger(std::filesystem::path log_dir, std::string name);

std::filesystem::path initializeOutputsDirectory(const std::string& output_dir,
                                                 const std::string& tag,
                                                 bool use_timestamp = true,
                                                 int flash_data_every_n = 10);

void installFailureSignalHandler();

inline void redirectCoutToLogger() {
  std::ostream cout(&SpdlogBuf::instance());
  std::cout.rdbuf(cout.rdbuf());
}

inline void flashData() {
  // Write the data to the file
  std::ofstream fout(dataFile);
  fout << dataRoot;
  fout.close();
  FlashDataCounter = 0;
}

template <typename Mutex>
class callback_sink : public spdlog::sinks::base_sink<Mutex> {
 public:
  using log_func_t = std::function<void(const spdlog::details::log_msg&)>;

  explicit callback_sink(log_func_t log_func)
      : log_func_(std::move(log_func)) {}

 protected:
  void sink_it_(const spdlog::details::log_msg& msg) override {
    log_func_(msg);
  }

  void flush_() override {
    // No-op for this sink
  }

 private:
  log_func_t log_func_;
};

using callback_sink_mt = callback_sink<std::mutex>;  // Thread-safe sink
using callback_sink_st =
    callback_sink<spdlog::details::null_mutex>;  // Non-thread-safe sink

}  // namespace aria::logging

#endif  // ARIA_COMMON_LOGGING_H_