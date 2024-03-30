#ifndef ARIA_DOPT_ROS_ROSBAG_READER_H_
#define ARIA_DOPT_ROS_ROSBAG_READER_H_

#include <ros/ros.h>
#include <rosbag/bag.h>

namespace aria {

class GenericCallback {
 public:
  virtual ~GenericCallback() {}
  virtual void call(rosbag::MessageInstance const m) = 0;
};

template <typename M>
class TypedCallback : public GenericCallback {
 public:
  using Callback = std::function<void(const boost::shared_ptr<M const>&)>;
  TypedCallback(Callback callback) : callback_(callback) {}

  void call(rosbag::MessageInstance const m) override {
    boost::shared_ptr<M const> msg = m.instantiate<M>();
    CHECK(msg != nullptr)
        << "Failed to instantiate message, are message defs consistent?";
    if (msg != nullptr) {
      callback_(msg);
    }
  }

 private:
  Callback callback_;
};

class RosbagReader {
 public:
  RosbagReader(const std::string& bag_file);

  template <typename M>
  void registerCallback(const std::string& topic,
                        typename TypedCallback<M>::Callback callback) {
    callbacks_[topic] = std::make_unique<TypedCallback<M>>(callback);
  }

  void readBag();

 private:
  std::string bag_file_path_;
  std::map<std::string, std::unique_ptr<GenericCallback>> callbacks_;
};
}  // namespace aria

#endif  // ARIA_DOPT_ROS_ROSBAG_READER_H_
