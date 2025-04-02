#ifndef ARIA_COMMON_BENCHMARK_H_
#define ARIA_COMMON_BENCHMARK_H_

#include <tbb/concurrent_hash_map.h>

#include <boost/timer/timer.hpp>
#include <map>
#include <optional>
#include <string>

#include "aria_common/logging.h"

namespace aria {
// Structure to store dynamic benchmark statistics
struct DynamicBenchmarkStats {
  std::string label;
  std::optional<int> index;
  double mean = 0.0;
  double m2 = 0.0;  // For variance calculation
  double max = 0.0;
  double min = 0.0;
  size_t count = 0;
  std::mutex mutex;
};

// Global map to store all benchmark statistics
extern tbb::concurrent_hash_map<std::string, DynamicBenchmarkStats>
    benchmarkStatsMap;

// Function to calculate and print benchmark statistics
std::string printBenchmarkStats();

void logTimingToData();

inline void clearBenchmarkStats() { benchmarkStatsMap.clear(); }

inline void updateBenchmarkStats(const std::string& label,
                                 std::optional<int> index,
                                 double duration) {
  tbb::concurrent_hash_map<std::string, DynamicBenchmarkStats>::accessor a;
  auto label_with_index = label;
  label_with_index += index.has_value() ? std::to_string(index.value()) : "_";
  if (benchmarkStatsMap.insert(a, label_with_index)) {
    a->second.label = label;
    a->second.index = index;
  }

  // Lock the mutex for thread-safe modification
  std::lock_guard<std::mutex> lock(a->second.mutex);

  auto& stats = a->second;
  stats.count++;
  double delta = duration - stats.mean;
  stats.mean += delta / stats.count;
  stats.m2 += delta * (duration - stats.mean);

  if (duration > stats.max) {
    stats.max = duration;
  }

  if (duration < stats.min || stats.min == 0) {
    stats.min = duration;
  }
}

class Timer {
 public:
  Timer(const std::string& label, int index = -1)
      : label_(label), index_(index) {
    timer_.start();
  }
  ~Timer() {
    if (timer_.is_stopped()) {
      return;
    }
    stop();
  }
  void stop() {
    timer_.stop();
    aria::updateBenchmarkStats(label_, index_, elapsed());
  }
  double elapsed() const { return timer_.elapsed().wall / 1e6; }

 public:
  std::string label_;
  int index_;
  boost::timer::cpu_timer timer_;
};

#define BENCHMARK(codeBlock, label, index)                  \
  do {                                                      \
    std::string label_str(label);                           \
    Timer timer(label_str, index);                          \
    try {                                                   \
      codeBlock;                                            \
    } catch (const std::exception& e) {                     \
      spdlog::error("Benchmarking: " + label_str +          \
                    " failed with exception: " + e.what()); \
      throw;                                                \
    }                                                       \
  } while (0)

}  // namespace aria

#endif  // ARIA_COMMON_BENCHMARK_H_
