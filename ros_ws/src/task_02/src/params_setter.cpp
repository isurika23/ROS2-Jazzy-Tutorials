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
