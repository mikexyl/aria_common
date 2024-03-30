#ifndef ARIA_COMMON_BENCHMARK_H_
#define ARIA_COMMON_BENCHMARK_H_

// Macro for benchmarking a block of code
#define BENCHMARK(codeBlock, label)                                        \
  do {                                                                     \
    LOG(INFO) << "Benchmarking: " << label;                                \
    auto start = std::chrono::high_resolution_clock::now();                \
    try {                                                                  \
      codeBlock;                                                           \
    } catch (const std::exception& e) {                                    \
      LOG(ERROR) << "Benchmarking: " << label                              \
                 << " failed with exception: " << e.what();                \
      throw;                                                               \
    }                                                                      \
    auto end = std::chrono::high_resolution_clock::now();                  \
    auto duration =                                                        \
        std::chrono::duration_cast<std::chrono::microseconds>(end - start) \
            .count();                                                      \
    LOG(INFO) << label << " took " << duration << " microseconds.";        \
  } while (0)

namespace aria {}

#endif  // ARIA_COMMON_BENCHMARK_H_
