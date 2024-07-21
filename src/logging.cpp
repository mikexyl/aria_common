#include "aria_common/logging.h"

#include <gtsam/geometry/Pose3.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <csignal>
#include <filesystem>
#include <fstream>

namespace aria::logging {

void initializeLogger(std::filesystem::path log_dir, std::string name) {
  // Create sinks for each level of logging you need
  auto debug_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      log_dir / (name + "_debug.log"), true);
  auto info_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      log_dir / (name + "_info.log"), true);
  auto warn_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      log_dir / (name + "_warn.log"), true);
  auto error_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      log_dir / (name + "_error.log"), true);
  auto fatal_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      log_dir / (name + "_fatal.log"), true);

  // Set the level for each sink
  debug_sink->set_level(spdlog::level::trace);
  info_sink->set_level(spdlog::level::info);
  warn_sink->set_level(spdlog::level::warn);
  error_sink->set_level(spdlog::level::err);
  fatal_sink->set_level(spdlog::level::critical);

  // Create a console sink
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::info);

  // Create a logger with multiple sinks
  std::vector<spdlog::sink_ptr> sinks{
      debug_sink, info_sink, warn_sink, error_sink, fatal_sink, console_sink};
  auto logger =
      // TODO: the logger's name probably should be the experiment name
      std::make_shared<spdlog::logger>(name, begin(sinks), end(sinks));
  logger->set_level(spdlog::level::debug);
  logger->flush_on(
      spdlog::level::info);  // Only flush on errors (not info or debug

  // Register it globally
  spdlog::register_logger(logger);
  spdlog::set_default_logger(logger);

  std::vector<spdlog::sink_ptr> fatal_sinks{fatal_sink, console_sink};
  auto fatal_logger = std::make_shared<spdlog::logger>(
      "failure_signal_logger", begin(fatal_sinks), end(fatal_sinks));
  fatal_logger->set_level(spdlog::level::critical);
  fatal_logger->flush_on(spdlog::level::critical);
  spdlog::register_logger(fatal_logger);

  // register a data output logger
  auto data_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      log_dir / (name + "_data.log"), true);
  data_sink->set_level(spdlog::level::debug);

  std::vector<spdlog::sink_ptr> data_sinks{data_sink, console_sink};
  auto data_logger = std::make_shared<spdlog::logger>(
      "data_logger", begin(data_sinks), end(data_sinks));
  spdlog::register_logger(data_logger);

  spdlog::flush_every(std::chrono::seconds(3));  // Auto-flush every 3 seconds
  spdlog::set_pattern(
      "[%Y-%m-%d %H:%M:%S.%e] [%^%L%$] %v");  // Custom log pattern
}

void handleFailureSignal(int signal) {
  auto fatal_logger = spdlog::get("failure_signal_logger");
  if (fatal_logger) {
    fatal_logger->critical("Fatal signal: {}", signal);
  }

  std::_Exit(EXIT_FAILURE);
}

void installFailureSignalHandler() {
  std::signal(SIGSEGV, handleFailureSignal);
  std::signal(SIGABRT, handleFailureSignal);
  std::signal(SIGFPE, handleFailureSignal);
  std::signal(SIGILL, handleFailureSignal);
}

std::filesystem::path initializeOutputsDirectory(const std::string& output_dir,
                                                 const std::string& tag,
                                                 bool use_timestamp) {
  // Get a timestamp
  auto now = std::chrono::system_clock::now();
  auto timestamp = std::chrono::system_clock::to_time_t(now);

  // Create a directory path with the timestamp
  std::filesystem::path log_dir = std::filesystem::path(output_dir);
  if (use_timestamp) log_dir /= std::to_string(timestamp);

  // Create the directory and any necessary parent directories
  std::filesystem::create_directories(log_dir / "logs");
  std::filesystem::create_directories(log_dir / "graphs");

  // Create a dummy file with the tag as the name
  std::ofstream tag_file((log_dir / tag));

  initializeLogger(log_dir / "logs", tag);

  return log_dir;
}

}  // namespace aria::logging