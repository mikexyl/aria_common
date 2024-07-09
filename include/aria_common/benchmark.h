#ifndef ARIA_COMMON_BENCHMARK_H_
#define ARIA_COMMON_BENCHMARK_H_

#include <boost/timer/timer.hpp>
#include <map>
#include <string>

#include "aria_common/logging.h"

namespace aria {
// Structure to store dynamic benchmark statistics
struct DynamicBenchmarkStats {
  std::string label;
  double mean = 0.0;
  double m2 = 0.0;  // For variance calculation
  size_t count = 0;
};

// Global map to store all benchmark statistics
extern std::map<std::string, DynamicBenchmarkStats> benchmarkStatsMap;

// Function to calculate and print benchmark statistics
std::string printBenchmarkStats();

inline void updateBenchmarkStats(const std::string& label, double duration) {
  double duration_ms = duration / 1e6;
  auto& stats = benchmarkStatsMap[label];
  stats.label = label;
  stats.count++;
  double delta = duration_ms - stats.mean;
  stats.mean += delta / stats.count;
  stats.m2 += delta * (duration_ms - stats.mean);
}

#define BENCHMARK(codeBlock, label)                                        \
  do {                                                                     \
    std::string label_str(label);                                          \
    spdlog::info("Benchmarking: " + label_str);                            \
    boost::timer::cpu_timer timer;                                         \
    try {                                                                  \
      codeBlock;                                                           \
    } catch (const std::exception& e) {                                    \
      spdlog::error("Benchmarking: " + label_str +                         \
                    " failed with exception: " + e.what());                \
      throw;                                                               \
    }                                                                      \
    timer.stop();                                                          \
    auto duration = timer.elapsed().wall;                                  \
    spdlog::info(label_str + " took " + std::to_string(duration) + " ns"); \
    updateBenchmarkStats(label, duration);                                 \
  } while (0)

}  // namespace aria

#endif  // ARIA_COMMON_BENCHMARK_H_
