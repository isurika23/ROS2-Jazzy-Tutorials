// to see the publishing of twist messages from a CSV file in ROS2.

// Open a new terminal. After sourcing the workspace, run the following command:
// >> ros2 run task_03 twist_from_database

// To see the published messages, you can use the following command in another terminal:
// >> ros2 topic echo /twist_from_database

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

// Global variable to store twist values
// This will hold a vector of vectors, where each inner vector represents a row of values from the CSV file.
std::vector<std::vector<float>> twist_values;

class TwistFromDatabase : public rclcpp::Node
{
public:
    TwistFromDatabase() : Node("twist_from_database"), count_(0)
    {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("twist_from_database", 20);
        RCLCPP_INFO(this->get_logger(), "Publisher created on topic 'twist_from_database'");

        // std::cout << count_ << std::endl;

        auto timer_callback = [this]()
        {
            // Create a Twist message for each row of values
            if (this->count_ >= twist_values.size())
            {
                RCLCPP_INFO(this->get_logger(), "All values have been published.");
                return; // Stop publishing if all values are sent
            }
            // Create a message with the current count
            geometry_msgs::msg::Twist twist_msg;
            twist_msg.linear.x = twist_values[count_][0];
            twist_msg.linear.y = twist_values[count_][1];
            twist_msg.linear.z = twist_values[count_][2];
            twist_msg.angular.x = twist_values[count_][3];
            twist_msg.angular.y = twist_values[count_][4];
            twist_msg.angular.z = twist_values[count_][5];
            message_num = this->count_++;
            RCLCPP_INFO_STREAM(this->get_logger(), "Publishing: '" << message_num << "'");
            publisher_->publish(twist_msg);
        };
        timer_ = this->create_wall_timer(100ms, timer_callback);
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    size_t count_; // Counter to keep track of the number of messages published
    int message_num; // Variable to store the current message number
};

int main(int argc, char *argv[])
{
    RCLCPP_INFO_STREAM(rclcpp::get_logger("main"), "Current working dir: " << std::filesystem::current_path());
    std::ifstream file("./src/task_03/src/values.csv"); // Open the CSV file
    std::string line;

    // std::cout << "twist_values size: " << twist_values.size() << std::endl;
    RCLCPP_INFO_STREAM(rclcpp::get_logger("main"), "twist_values size: " << twist_values.size() << "'");

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string value;
        std::vector<float> values;

        while (std::getline(ss, value, ','))
        {
            // Remove leading/trailing spaces
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            values.push_back(std::stof(value)); // Convert to float
        }

        // Store the values in twist_values
        twist_values.push_back(values);

        // Example: print the values
        // for (float v : values) {
        //     std::cout << v << " ";
        // }
        // std::cout << std::endl;
    }
    // std::cout << "twist_values size after loading: " << twist_values.size() << std::endl;
    // RCLCPP_INFO_STREAM(rclcpp::get_logger("main"), "twist_values size after loading: " << twist_values.size() << "'");

    file.close();

    // std::cout << "Loaded twist values from CSV file. size: " << twist_values.size() << std::endl;
    RCLCPP_INFO_STREAM(rclcpp::get_logger("main"), "Loaded twist values from CSV file. size: " << twist_values.size() << "'");

    // for (const auto &row : twist_values)
    // {
    //     for (float value : row)
    //     {
    //         std::cout << value << " ";
    //     }
    //     std::cout << std::endl;
    // }

    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TwistFromDatabase>());
    rclcpp::shutdown();
    return 0;
}