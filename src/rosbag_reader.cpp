#include "aria_common/rosbag_reader.h"

#include "aria_common/logging.h"

namespace aria {

void RosbagReader::readBag() {
  openBag();

  while (readOnce());

  closeBag();
}
}  // namespace aria