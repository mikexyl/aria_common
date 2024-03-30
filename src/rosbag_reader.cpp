#include "aria_common/rosbag_reader.h"

#include <rosbag/bag.h>
#include <rosbag/view.h>

#include <algorithm>

#include "aria_common/logging.h"

namespace aria {
RosbagReader::RosbagReader(const std::string& bag_file)
    : bag_file_path_(bag_file), view_(nullptr) {}

void RosbagReader::readBag() {
  openBag();

  CHECK(view_ != nullptr) << "View is not initialized";
  while (readOnce());

  closeBag();
}
}  // namespace aria