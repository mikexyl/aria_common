#ifndef ARIA_COMMON_BENCHMARK_H_
#define ARIA_COMMON_BENCHMARK_H_

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
    auto& stats = benchmarkStatsMap[label_str];                            \
    stats.label = label_str;                                               \
    stats.count++;                                                         \
    double delta = duration - stats.mean;                                  \
    stats.mean += delta / stats.count;                                     \
    stats.m2 += delta * (duration - stats.mean);                           \
  } while (0)

}  // namespace aria

#endif  // ARIA_COMMON_BENCHMARK_H_
