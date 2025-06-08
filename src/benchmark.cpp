#include "aria_common/benchmark.h"

#include <tbb/concurrent_hash_map.h>

namespace aria {

// Global map to store all benchmark statistics
tbb::concurrent_hash_map<std::string, DynamicBenchmarkStats> benchmarkStatsMap;

// Function to calculate and print benchmark statistics
std::string printBenchmarkStats() {
  std::stringstream ss;
  ss << std::left << std::setw(20) << "Label" << std::setw(5) << "Index"
     << std::setw(15) << "Mean (ms)" << std::setw(15) << "Std Dev (ms)"
     << std::setw(15) << "Max (ms)" << std::setw(15) << "Min (ms)"
     << std::setw(10) << "Runs" << std::setw(10) << "Total (ms)" << std::endl;
  ss << std::string(60, '-') << std::endl;

  tbb::concurrent_hash_map<std::string, DynamicBenchmarkStats>::const_accessor
      c_accessor;

  // get all keys from the map and sort
  std::vector<std::string> keys;

  for (auto it = benchmarkStatsMap.begin(); it != benchmarkStatsMap.end();
       ++it) {
    keys.push_back(it->first);
  }

  std::sort(keys.begin(), keys.end());

  for (auto key : keys) {
    if (benchmarkStatsMap.find(c_accessor, key)) {
      const auto& stats = c_accessor->second;
      double variance = stats.count > 1 ? stats.m2 / (stats.count - 1) : 0.0;
      double stddev = std::sqrt(variance);

      std::string index =
          stats.index.has_value() ? std::to_string(stats.index.value()) : "-";

      double total = stats.mean * stats.count;

      ss << std::left << std::setw(20) << stats.label << std::setw(5)
         << std::fixed << index << std::setw(15) << std::fixed
         << std::setprecision(2) << stats.mean << std::setw(15) << std::fixed
         << std::setprecision(2) << stddev << std::setw(15) << std::fixed
         << std::setprecision(2) << stats.max << std::setw(15) << std::fixed
         << std::setprecision(2) << stats.min << std::setw(10) << stats.count
         << std::setw(10) << std::fixed << std::setprecision(2) << total
         << std::endl;
    }
  }

  return ss.str();
}

void logTimingToData() {
  // traverse the map and log the data
  for (auto it = benchmarkStatsMap.begin(); it != benchmarkStatsMap.end();
       ++it) {
    // log the data
    double total_time = it->second.mean * it->second.count;
    std::string label =
        it->second.label +
        std::string(it->second.index.has_value()
                        ? std::to_string(it->second.index.value())
                        : "_");
    LOG_DATA("timing/" + label, total_time);
  }
}

}  // namespace aria