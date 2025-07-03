#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/string.hpp"

class ZeroTwist : public rclcpp::Node
{
public:
    ZeroTwist() : Node("zero_twist")
    {
        auto is_stopped_callback_ =
            [this](const std_msgs::msg::String::SharedPtr msg) -> void {
                if(msg->data == "true")
                {
                    RCLCPP_INFO(rclcpp::get_logger("IS_STOPPED_Subscriber"), "Robot is stopped, publishing zero twist.");
                    geometry_msgs::msg::Twist twist_msg;
                    twist_msg.linear.x = 0.0;
                    twist_msg.linear.y = 0.0;
                    twist_msg.linear.z = 0.0;
                    twist_msg.angular.x = 0.0;
                    twist_msg.angular.y = 0.0;
                    twist_msg.angular.z = 0.0;
                    publisher_->publish(twist_msg);
                    RCLCPP_INFO(this->get_logger(), "Published zero twist message.");
                }
        };
        
        // Create a publishers and subscriptions
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("twist", 10);
        subscription_ = this->create_subscription<std_msgs::msg::String>(
            "is_stopped", 10, is_stopped_callback_);
        
        RCLCPP_INFO(this->get_logger(), "ZeroTwist node has been started.");
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ZeroTwist>());
    rclcpp::shutdown();
    return 0;
}

// Instructions to run the node:
// 1. Ensure you have sourced your ROS 2 workspace.
// 2. Build the package using `colcon build`.
// 3. Source the setup file: `source install/setup.bash`.
// 4. Run the node using: `ros2 run task_04 zero_twist`.
// 5. To test, publish a message to the `is_stopped` topic with the content "true" using:
//    >> ros2 topic pub /is_stopped std_msgs/msg/String "{data: 'true'}"
// 6. You should see the zero twist message being published on the `twist` topic.
// 7. To view the published messages, you can use:
//    >> ros2 topic echo /twist 