#ifndef ARIA_DOPT_ROS_ROSBAG_WRITER_H_
#define ARIA_DOPT_ROS_ROSBAG_WRITER_H_

// src/recorder.cpp
#include <ros/ros.h>
#include <rosbag/bag.h>
#include <std_msgs/String.h>

#include <string>
#include <vector>

namespace aria {

class RosbagWriter {
 public:
  RosbagWriter(const std::string& bag_file_name) {
    bag_.open(bag_file_name, rosbag::bagmode::Write);
  }

  ~RosbagWriter() { bag_.close(); }

  template <typename M>
  void addMessage(const std::string& topic,
                  const M& message,
                  const ros::Time& time = ros::Time::now()) {
    // check bag is valid
    if (bag_.isOpen()) {
      bag_.write(topic, time, message);
    } else {
      std::cerr << "Bag is not open!" << std::endl;
    }
  }

 private:
  rosbag::Bag bag_;
};

}  // namespace aria

#endif  // ARIA_DOPT_ROS_ROSBAG_WRITER_H_
