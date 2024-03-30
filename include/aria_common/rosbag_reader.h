#ifndef ARIA_DOPT_ROS_ROSBAG_READER_H_
#define ARIA_DOPT_ROS_ROSBAG_READER_H_

#include <glog/logging.h>
#include <ros/ros.h>
#include <rosbag/bag.h>
#include <rosbag/view.h>

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

  void openBag() {
    try {
      bag_.open(bag_file_path_, rosbag::bagmode::Read);
      // print all topics
    } catch (const rosbag::BagException& e) {
      LOG(FATAL) << "Failed to open bag file: " << e.what();
    }

    std::vector<std::string> topics;
    for (const auto& cb : callbacks_) {
      topics.push_back(cb.first);
    }

    view_.reset(new rosbag::View(bag_, rosbag::TopicQuery(topics)));
    it_ = view_->begin();
  }

  void readBag();

  void closeBag() { bag_.close(); }

  bool readOnce() {
    const rosbag::MessageInstance& m = *it_;
    VLOG(1) << "Received message on topic: " << m.getTopic();
    if (!shutdown_ and callbacks_.count(m.getTopic())) {
      VLOG(1) << "calling callback for topic: " << m.getTopic();
      CHECK(callbacks_[m.getTopic()] != nullptr);
      callbacks_[m.getTopic()]->call(m);
    }
    ++it_;

    return it_ != view_->end();
  }

  void shutdown() {
    shutdown_ = true;
    closeBag();
    view_.reset();
    it_ = rosbag::View::iterator();
  }

 private:
  std::string bag_file_path_;
  rosbag::Bag bag_;
  std::shared_ptr<rosbag::View> view_;
  rosbag::View::iterator it_;
  std::map<std::string, std::unique_ptr<GenericCallback>> callbacks_;

  std::atomic<bool> shutdown_{false};
};
}  // namespace aria

#endif  // ARIA_DOPT_ROS_ROSBAG_READER_H_
