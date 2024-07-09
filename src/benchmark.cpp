#include "aria_common/benchmark.h"

namespace aria {

// Global map to store all benchmark statistics
std::map<std::string, DynamicBenchmarkStats> benchmarkStatsMap;

// Function to calculate and print benchmark statistics
std::string printBenchmarkStats() {
    std::stringstream ss;
    ss << std::left << std::setw(20) << "Label"
       << std::setw(15) << "Mean (ns)"
       << std::setw(15) << "Std Dev (ns)"
       << std::setw(10) << "Runs" << std::endl;
    ss << std::string(60, '-') << std::endl;

    for (const auto& entry : benchmarkStatsMap) {
        const auto& stats = entry.second;
        double variance = stats.count > 1 ? stats.m2 / (stats.count - 1) : 0.0;
        double stddev = std::sqrt(variance);

        ss << std::left << std::setw(20) << stats.label
           << std::setw(15) << std::fixed << std::setprecision(2) << stats.mean
           << std::setw(15) << std::fixed << std::setprecision(2) << stddev
           << std::setw(10) << stats.count << std::endl;
    }

    return ss.str();
}
}  // namespace aria