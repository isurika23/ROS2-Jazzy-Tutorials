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
