#ifndef ARIA_COMMON_BENCHMARK_H_
#define ARIA_COMMON_BENCHMARK_H_

#include "aria_common/logging.h"

// Macro for benchmarking a block of code
#define BENCHMARK(codeBlock, label)                                        \
  do {                                                                     \
    std::string label_str(label);                                          \
    spdlog::info("Benchmarking: " + label_str);                            \
    auto start = std::chrono::high_resolution_clock::now();                \
    try {                                                                  \
      codeBlock;                                                           \
    } catch (const std::exception& e) {                                    \
      spdlog::error("Benchmarking: " + label_str +                         \
                    " failed with exception: " + e.what());                \
      throw;                                                               \
    }                                                                      \
    auto end = std::chrono::high_resolution_clock::now();                  \
    auto duration =                                                        \
        std::chrono::duration_cast<std::chrono::microseconds>(end - start) \
            .count();                                                      \
    spdlog::info(label_str + " took " + std::to_string(duration) +         \
                 " microseconds.");                                        \
  } while (0)

namespace aria {}

#endif  // ARIA_COMMON_BENCHMARK_H_
