#include "aria_common/logging.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <csignal>
#include <filesystem>
#include <fstream>

namespace aria::logging {

void initialize_logger(std::filesystem::path log_dir, std::string name) {
  // Create sinks for each level of logging you need
  auto info_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      log_dir / (name + "_info.log"), true);
  auto warn_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      log_dir / (name + "_warn.log"), true);
  auto error_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      log_dir / (name + "_error.log"), true);
  auto fatal_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      log_dir / (name + "_fatal.log"), true);

  // Set the level for each sink
  info_sink->set_level(spdlog::level::info);
  warn_sink->set_level(spdlog::level::warn);
  error_sink->set_level(spdlog::level::err);
  fatal_sink->set_level(spdlog::level::critical);

  // Create a console sink
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

  // Create a logger with multiple sinks
  std::vector<spdlog::sink_ptr> sinks{
      info_sink, warn_sink, error_sink, console_sink};
  auto logger =
      // TODO: the logger's name probably should be the experiment name
      std::make_shared<spdlog::logger>(name, begin(sinks), end(sinks));

  // Register it globally
  spdlog::register_logger(logger);
  spdlog::set_default_logger(logger);

  std::vector<spdlog::sink_ptr> fatal_sinks{fatal_sink, console_sink};
  auto fatal_logger = std::make_shared<spdlog::logger>(
      "failure_signal_logger", begin(fatal_sinks), end(fatal_sinks));
  spdlog::register_logger(fatal_logger);
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
                                                 const std::string& tag) {
  // Get a timestamp
  auto now = std::chrono::system_clock::now();
  auto result = std::chrono::system_clock::to_time_t(now);

  // Create a directory path with the timestamp
  std::filesystem::path log_dir =
      std::filesystem::path(output_dir) / std::to_string(result);

  // Create the directory and any necessary parent directories
  std::filesystem::create_directories(log_dir / "logs");
  std::filesystem::create_directories(log_dir / "graphs");

  // Create a dummy file with the tag as the name
  std::ofstream tag_file((log_dir / tag));

  // Initialize spdlog with file sink
  auto file_logger = spdlog::basic_logger_mt(
      "file_logger", (log_dir / "logs" / "log.txt").string());

  // Set spdlog configuration
  file_logger->set_level(spdlog::level::info);  // Setting log level
  spdlog::set_default_logger(file_logger);  // Setting this as default logger
  spdlog::flush_every(std::chrono::seconds(3));  // Auto-flush every 3 seconds
  spdlog::set_pattern(
      "[%Y-%m-%d %H:%M:%S.%e] [%^%L%$] %v");  // Custom log pattern

  initialize_logger(log_dir / "logs", tag);

  return log_dir;
}

}  // namespace aria::logging