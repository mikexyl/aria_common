#include "aria_common/rosbag_reader.h"

#include <rosbag/bag.h>
#include <rosbag/view.h>

#include <algorithm>

#include "aria_common/logging.h"

namespace aria {
RosbagReader::RosbagReader(const std::string& bag_file)
    : bag_file_path_(bag_file) {}

void RosbagReader::readBag() {
  rosbag::Bag bag;
  try {
    bag.open(bag_file_path_, rosbag::bagmode::Read);
    // print all topics
  } catch (const rosbag::BagException& e) {
    LOG(FATAL) << "Failed to open bag file: " << e.what();
  }

  std::vector<std::string> topics;
  for (const auto& cb : callbacks_) {
    topics.push_back(cb.first);
  }

  rosbag::View view(bag, rosbag::TopicQuery(topics));

  std::for_each(
      view.begin(), view.end(), [this](const rosbag::MessageInstance& m) {
        VLOG(1) << "Received message on topic: " << m.getTopic();
        if (callbacks_.count(m.getTopic())) {
          VLOG(1) << "calling callback for topic: " << m.getTopic();
          CHECK(callbacks_[m.getTopic()] != nullptr);
          callbacks_[m.getTopic()]->call(m);
        }
      });

  bag.close();
}
}  // namespace aria