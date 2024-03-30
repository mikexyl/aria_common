#ifndef ARIA_COMMON_TYPE_H_
#define ARIA_COMMON_TYPE_H_

#include <cstddef>

namespace aria {
inline size_t secToNsec(float sec) { return static_cast<size_t>(sec * 1e9); }
inline float nsecToSec(size_t nsec) { return static_cast<float>(nsec / 1e9); }
}  // namespace aria

#endif  // ARIA_COMMON_TYPE_H_
