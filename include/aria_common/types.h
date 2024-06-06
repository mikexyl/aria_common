#ifndef ARIA_COMMON_TYPE_H_
#define ARIA_COMMON_TYPE_H_

#include <cstddef>
#include <cstdint>

namespace aria {
inline size_t secToNsec(float sec) { return static_cast<size_t>(sec * 1e9); }
inline float nsecToSec(size_t nsec) { return static_cast<float>(nsec / 1e9); }

// TODO: if we want to use MAC address or something to represent agent's id,
// uint8_t won't be enough
using AgentId = uint8_t;

}  // namespace aria

#endif  // ARIA_COMMON_TYPE_H_
