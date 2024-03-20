#include "aria_common/benchmark.h"
#include "aria_common/logging.h"
#include "aria_common/macros.h"

namespace aria {

class DependencyTracker {
 public:
  ARIA_POINTER_TYPEDEFS(DependencyTracker);
};

void testInitializeOutputsDirectory() { initializeOutputsDirectory("", ""); }

}  // namespace aria
