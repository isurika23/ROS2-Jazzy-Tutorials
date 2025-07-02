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