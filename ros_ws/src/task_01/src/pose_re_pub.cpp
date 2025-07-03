#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_with_covariance_stamped.hpp"
#include "geometry_msgs/msg/pose.hpp"

class PoseRePubNode : public rclcpp::Node
{
public:
    PoseRePubNode() : Node("pose_re_pub")
    {
        auto topic_callback_ =
            [this](const geometry_msgs::msg::PoseWithCovarianceStamped::SharedPtr msg) -> void {
            geometry_msgs::msg::Pose pose_msg = msg->pose.pose;
            publisher_->publish(pose_msg);
        };
        publisher_ = this->create_publisher<geometry_msgs::msg::Pose>("pose", 10);
        subscription_ = this->create_subscription<geometry_msgs::msg::PoseWithCovarianceStamped>(
            "pose_with_covariance_stamped", 10, topic_callback_);
        RCLCPP_INFO(this->get_logger(), "PoseRePubNode has been started.");
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Pose>::SharedPtr publisher_;
    rclcpp::Subscription<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr subscription_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<PoseRePubNode>());
    rclcpp::shutdown();
    return 0;
}

// to test the node, run the following commands in separate terminals:
// 1. `ros2 run task_01 pose_re_pub`
// 2. `ros2 topic pub /pose_with_covariance_stamped geometry_msgs/msg/PoseWithCovarianceStamped "
// header:
//   stamp:
//     sec: 0
//     nanosec: 0
//   frame_id: 'map'
// pose:
//   pose:
//     position:
//       x: 1.0
//       y: 2.0
//       z: 0.0
//     orientation:
//       x: 0.0
//       y: 0.0
//       z: 0.0
//       w: 1.0
//   covariance: [0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
//                0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
//                0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
//                0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
//                0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
//                0.0, 0.0, 0.0, 0.0, 0.0, 0.0]"
// "
// 3. `ros2 topic echo /pose`