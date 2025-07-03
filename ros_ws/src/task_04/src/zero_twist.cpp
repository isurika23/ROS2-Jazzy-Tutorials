#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/string.hpp"

class ZeroTwist : public rclcpp::Node
{
public:
    ZeroTwist() : Node("zero_twist"), count_(1)
    {
        auto is_stopped_callback_ =
            [this](const std_msgs::msg::String::SharedPtr msg) -> void {
                if(msg->data == "true")
                {
                    RCLCPP_INFO(rclcpp::get_logger("IS_STOPPED_Subscriber"), "Robot is stopped, publishing zero twist.");
                }
        };

        auto twist_callback_ =
            [this](const geometry_msgs::msg::Twist::SharedPtr msg) -> void {
                if (msg->linear.x == 0.0 && msg->linear.y == 0.0 && msg->linear.z == 0.0 &&
                    msg->angular.x == 0.0 && msg->angular.y == 0.0 && msg->angular.z == 0.0)
                {
                    msg_num = this->count_;
                    RCLCPP_INFO_STREAM(rclcpp::get_logger("ZeroTwist"), "Received zero twist '" << msg_num << "', publishing is_stopped message.");
                    std_msgs::msg::String is_stopped_msg;
                    is_stopped_msg.data = "true";
                    publisher_->publish(is_stopped_msg);
                    this->count_++;
                }
            };
        
        // Create a publishers and subscriptions
        publisher_ = this->create_publisher<std_msgs::msg::String>("is_stopped", 10);
        subscription_ = this->create_subscription<std_msgs::msg::String>(
            "is_stopped", 10, is_stopped_callback_);
        twist_subscription_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "twist_from_database", 10, twist_callback_);
        
        RCLCPP_INFO(this->get_logger(), "ZeroTwist node has been started.");
    }

private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr twist_subscription_;
    size_t count_; // Counter to keep track of the number of messages published
    int msg_num; // Variable to store the current message number
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ZeroTwist>());
    rclcpp::shutdown();
    return 0;
}