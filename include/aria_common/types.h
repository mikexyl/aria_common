#ifndef ARIA_COMMON_TYPE_H_
#define ARIA_COMMON_TYPE_H_

#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <set>
#include <unordered_map>
#include <utility>

namespace aria {
inline size_t secToNsec(float sec) { return static_cast<size_t>(sec * 1e9); }
inline float nsecToSec(size_t nsec) { return static_cast<float>(nsec / 1e9); }

// TODO: if we want to use MAC address or something to represent agent's id,
// uint8_t won't be enough
using AgentId = int;
using AgentIdPair = std::pair<AgentId, AgentId>;
using AgentIdSet = std::set<AgentId>;

struct AgentIdPairCompare {
  bool operator()(const AgentIdPair& lhs, const AgentIdPair& rhs) const {
    return lhs.first < rhs.first ||
           (lhs.first == rhs.first && lhs.second < rhs.second);
  }
};

struct AgentIdPairCompareBidirectional {
  bool operator()(const AgentIdPair& lhs, const AgentIdPair& rhs) const {
    return lhs.first * std::numeric_limits<AgentId>::max() + lhs.second <
           rhs.first * std::numeric_limits<AgentId>::max() + rhs.second;
  }
};

// Custom hash for AgentIdPair, order does not matter
struct AgentIdPairHash {
  size_t operator()(const AgentIdPair& agent_id_pair) const {
    // Get the agent IDs
    AgentId first = agent_id_pair.first;
    AgentId second = agent_id_pair.second;

    // Symmetric hash: the order of first and second doesn't matter
    return std::hash<AgentId>()(first + second) ^
           std::hash<AgentId>()(first * second);
  }
};

// Equality operator for AgentIdPair (for unordered_map to work properly)
struct AgentIdPairEqual {
  bool operator()(const AgentIdPair& lhs, const AgentIdPair& rhs) const {
    return (lhs.first == rhs.first && lhs.second == rhs.second) ||
           (lhs.first == rhs.second && lhs.second == rhs.first);
  }
};

template <typename T>
using AgentIdPairMap = std::map<AgentIdPair, T, AgentIdPairCompare>;

template <typename T>
using AgentIdPairMapBi =
    std::unordered_map<AgentIdPair, T, AgentIdPairHash, AgentIdPairEqual>;

using AgentIdPairSet = std::set<AgentIdPair, AgentIdPairCompare>;

}  // namespace aria

#endif  // ARIA_COMMON_TYPE_H_
