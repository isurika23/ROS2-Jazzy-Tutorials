## ROS2 Core Concepts - Report
---
**Name**: Isurika Gunasekara
**Index no.**: 220192J  

---

## ✅ Task Summary

| Task | Node Name            | Key Concepts                    |
|------|----------------------|---------------------------------|
| 1    | pose_re_pub          | Topic subscription & publication |
| 2    | params_setter        | Global parameters               |
| 3    | twist_from_database  | CSV file parsing, publishers    |
| 4    | zero_twist           | Conditional publishing, subs    |
| 5    | test_launch.py       | Launch files, remapping, pub    |
| 6    | param_reader         | Reading parameters              |

---

## 📄 Task Details

### 🔹 Task 1: `pose_re_pub`
- **Subscribes to**: `/pose_with_covariance_stamped`
- **Publishes to**: `/pose`
- **Conversion**: Extracts `Pose` from `PoseWithCovarianceStamped`
- **Tested using**: `ros2 topic pub` CLI tool

#### ➡️ Code:
```cpp

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

```

---

### 🔹 Task 2: `params_setter`
-  Sets a global ros parameter from variable `param` with value from variable `value`
#### ➡️ Code:

```cpp
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/parameter.hpp"

class ParamsSetter : public rclcpp::Node
{
public:
    ParamsSetter() : Node("params_setter")
    {
        // Declare parameters with default values
        this->declare_parameter<std::string>("param", "default_param");
        this->declare_parameter<std::string>("value", "new_value");

        // Retrieve them
        std::string param = this->get_parameter("param").as_string();
        std::string value = this->get_parameter("value").as_string();

        // Log what you're about to do
        RCLCPP_INFO(this->get_logger(), "\nparam = '%s'\nvalue = '%s'", 
                    param.c_str(), value.c_str());
        
        RCLCPP_INFO(this->get_logger(), "Setting global parameter param = '%s' to value '%s'", 
                    param.c_str(), value.c_str());

        // Set the parameter (in this node's scope)
        this->set_parameter(rclcpp::Parameter("param", value));

        // Confirm it's set
        rclcpp::Parameter p;
        if (this->get_parameter("param", p)) {
            RCLCPP_INFO(this->get_logger(), "Confirmed: param = '%s'",
                        p.as_string().c_str());
        } else {
            RCLCPP_WARN(this->get_logger(), "Failed to get parameter '%s'", param.c_str());
        }
    }
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ParamsSetter>());
    rclcpp::shutdown();
    return 0;
}

```

---

### 🔹 Task 3: `twist_from_database`
- Reads a CSV file of `Twist` values
- Publishes each line at **10Hz** on `/twist_from_database`
- File: `values.csv`

#### ➡️ Code:
```cpp
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

    }
    
    file.close();
    
    RCLCPP_INFO_STREAM(rclcpp::get_logger("main"), "Loaded twist values from CSV file. size: " << twist_values.size() << "'");
    
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TwistFromDatabase>());
    rclcpp::shutdown();
    return 0;
}

```

---

### 🔹 Task 4: `zero_twist`
- **Subscribes to**: `/is_stopped` (std_msgs/String)
- **Publishes zero Twist** if message is `"true"`

➡️ Code: [task_4_zero_twist/zero_twist.py](task_4_zero_twist/zero_twist.py)
```cpp
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
```

---

### 🔹 Task 5: Launch + Nodes
- Launches `chatter.py` and `listener.py` with correct remapping
- Publishes a one-time `Twist` message via `ExecuteProcess`

#### ➡️ Launch file:
```py
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='tests',
            executable='chatter',
            name='test_chatter',
            output='screen',
            remappings={
                ('/chatter', '/test/chatter'),
            },
        ),
        Node(
            package='tests',
            executable='listener',
            name='listener',
            output='screen',
        ),

        ExecuteProcess(
            cmd=[
                'ros2', 'topic', 'pub', '--once', '/twist', 'geometry_msgs/msg/Twist',
                '{linear: {x: 0.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.0}}'
            ],
            output='screen'
        ),
    ])
```

---

### 🔹 Task 6: `param_reader`
- Reads parameter `robot_name` and prints it on startup

#### ➡️ Code:
```py
import rclpy
from rclpy.node import Node

class ParamReaderNode(Node):
    def __init__(self):
        super().__init__('param_reader')
        self.declare_parameter("robot_name", "LYSIA")
        robot_name = self.get_parameter("robot_name").get_parameter_value().string_value
        self.get_logger().info(f'Robot name is: {robot_name}')

def main(args=None):
    rclpy.init(args=args)
    node = ParamReaderNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()

```

---

## 🧪 Testing Notes

- All nodes tested individually using CLI tools:  
  `ros2 topic pub`, `ros2 topic echo`
- Launch file tested using:  
  `ros2 launch task_05 launch.py`

---

#### GitHub link:

[branch: core-concepts-isurika](https://github.com/isurika23/ROS2-Jazzy-Tutorials.git)