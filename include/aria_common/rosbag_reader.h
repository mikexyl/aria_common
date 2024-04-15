#ifndef ARIA_DOPT_ROS_ROSBAG_READER_H_
#define ARIA_DOPT_ROS_ROSBAG_READER_H_

#include <functional>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/serialization.hpp>
#include <rosbag2_cpp/readers/sequential_reader.hpp>
#include <rosbag2_cpp/typesupport_helpers.hpp>

namespace aria {

template <typename M>
using TypedCallback = std::function<void(const std::shared_ptr<M>)>;

class RosbagReader : public rclcpp::Node {
 public:
  explicit RosbagReader(const std::string& bag_file)
      : Node("rosbag2_reader"), bag_file_path_(bag_file) {}

  template <typename M>
  void registerCallback(const std::string& topic, TypedCallback<M> callback) {
    callbacks_[topic] =
        [callback](std::shared_ptr<rosbag2_storage::SerializedBagMessage> msg) {
          // convert serialized bag message to a serialized message
          rclcpp::SerializedMessage serialized_msg(*msg->serialized_data);

          // deserialize the message
          auto typed_msg = std::make_shared<M>();
          rclcpp::Serialization<M> serialization;
          serialization.deserialize_message(&serialized_msg, typed_msg.get());
          callback(typed_msg);
        };
  }

  void openBag() {
    try {
      auto storage_options = rosbag2_storage::StorageOptions();
      storage_options.uri = bag_file_path_;
      storage_options.storage_id = "sqlite3";
      auto converter_options = rosbag2_cpp::ConverterOptions();
      reader_.open(storage_options, converter_options);
      // Listing topics is a more manual process in rosbag2
    } catch (const std::runtime_error& e) {
      RCLCPP_FATAL(this->get_logger(), "Failed to open bag file: %s", e.what());
    }
  }

  void readBag();

  void closeBag() { reader_.close(); }

  bool readOnce() {
    if (reader_.has_next()) {
      auto message = reader_.read_next();

      if (callbacks_.find(message->topic_name) != callbacks_.end()) {
        RCLCPP_INFO(this->get_logger(),
                    "Received message on topic: %s",
                    message->topic_name.c_str());
        callbacks_[message->topic_name](message);
      }

      return true;
    }

    return false;
  }

  void shutdown() {
    this->shutdown_ = true;
    closeBag();
  }

 private:
  std::string bag_file_path_;
  rosbag2_cpp::readers::SequentialReader reader_;
  std::map<std::string,
           std::function<void(
               std::shared_ptr<rosbag2_storage::SerializedBagMessage>)>>
      callbacks_;
  std::atomic<bool> shutdown_{false};
};

}  // namespace aria

#endif  // ARIA_DOPT_ROS_ROSBAG_READER_H_
