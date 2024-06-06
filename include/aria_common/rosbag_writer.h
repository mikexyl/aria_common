#ifndef ARIA_DOPT_ROS_ROSBAG_WRITER_H_
#define ARIA_DOPT_ROS_ROSBAG_WRITER_H_

#include <iostream>
#include <memory>
#include <rclcpp/clock.hpp>
#include <rclcpp/serialization.hpp>
#include <rclcpp/serialized_message.hpp>
#include <rclcpp/time.hpp>
#include <rosbag2_cpp/typesupport_helpers.hpp>
#include <rosbag2_cpp/writers/sequential_writer.hpp>
#include <rosidl_runtime_cpp/message_type_support_decl.hpp>
#include <string>

namespace aria {

class RosbagWriter {
 public:
  explicit RosbagWriter(const std::string& bag_file_name) {
    auto writer_options = rosbag2_storage::StorageOptions();
    writer_options.uri = bag_file_name;
    writer_options.storage_id = "sqlite3";
    auto converter_options = rosbag2_cpp::ConverterOptions();
    writer_.open(writer_options, converter_options);
  }

  template <typename M>
  void addMessage(
      const std::string& topic,
      const std::string& type,
      const M& message,
      const rclcpp::Time& time = rclcpp::Clock(RCL_SYSTEM_TIME).now()) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (created_topics_.find(topic) == created_topics_.end()) {
      auto topic_meta_data = rosbag2_storage::TopicMetadata();
      topic_meta_data.name = topic;
      // get type string
      topic_meta_data.type = type;
      writer_.create_topic(topic_meta_data);
      created_topics_.insert(topic);
    }

    rclcpp::Serialization<M> serialization;
    rclcpp::SerializedMessage serialized_msg;
    serialization.serialize_message(&message, &serialized_msg);

    std::shared_ptr<rosbag2_storage::SerializedBagMessage> bag_message(
        new rosbag2_storage::SerializedBagMessage());
    bag_message->topic_name = topic;
    bag_message->time_stamp = time.nanoseconds();
    bag_message->serialized_data = std::shared_ptr<rcutils_uint8_array_t>(
        &serialized_msg.get_rcl_serialized_message(),
        [](rcutils_uint8_array_t* /*unused*/) {});

    writer_.write(bag_message);
  }

  ~RosbagWriter() { writer_.close(); }

 private:
  rosbag2_cpp::writers::SequentialWriter writer_;

  std::set<std::string> created_topics_;

  std::mutex mutex_;
};

}  // namespace aria

#endif  // ARIA_DOPT_ROS_ROSBAG_WRITER_H_
