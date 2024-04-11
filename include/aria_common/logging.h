#ifndef ARIA_COMMON_LOGGING_H_
#define ARIA_COMMON_LOGGING_H_

#include <glog/logging.h>

#include <filesystem>
#include <fstream>

#define HL(msg) (std::string("<: ") + (msg) + std::string(" /:>").c_str())

namespace aria {
inline std::filesystem::path initializeOutputsDirectory(
    const std::string& output_dir,
    const std::string& tag) {
  // get a timestamp
  auto now = std::chrono::system_clock::now();
  auto result = std::chrono::system_clock::to_time_t(now);
  // Create a directory path with the timestamp
  auto log_dir = std::filesystem::path(output_dir) / std::to_string(result);
  // Create the directory and any necessary parent directories
  std::filesystem::create_directories(log_dir / "logs");
  std::filesystem::create_directories(log_dir / "graphs");
  // Create a dummy file with the tag as the name
  std::ofstream tag_file((log_dir / tag).string());

  FLAGS_log_dir = (log_dir / "logs").string();
  // Set the logging flags before initialization
  FLAGS_alsologtostderr = true;
  FLAGS_colorlogtostderr = true;

  return log_dir;
}
}  // namespace aria

#endif  // ARIA_COMMON_LOGGING_H_