// Copyright 2016 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"
using std::placeholders::_1;

class BatterySubscriber : public rclcpp::Node
{
public:
    BatterySubscriber()
    : Node("battery_subscriber")
    {
    
    	this->declare_parameter("min_voltage", 35.0);
    	this->declare_parameter("max_voltage", 40.0);
    	
        // Subscriber na topic battery_voltage
        subscription_ = this->create_subscription<std_msgs::msg::Float32>(
            "battery_voltage", 10,
            std::bind(&BatterySubscriber::battery_callback, this, _1)
        );
        
        publisher_ = this->create_publisher<std_msgs::msg::Float32>("/battery_percentage", 10);

        // Předpokládáme, že Publisher už existuje někde jinde v Node
        // Pokud je součástí stejné Node, pak by zde byl jen SharedPtr ke Publisheru
    }

private:
    void battery_callback(const std_msgs::msg::Float32::SharedPtr msg)
    {
        auto min_voltage = this->get_parameter("min_voltage").as_double();
        auto max_voltage = this->get_parameter("max_voltage").as_double();
        
        float voltage = msg->data;
        float percentage = (voltage - min_voltage) / (max_voltage - min_voltage) * 100.0f;

        // oříznutí na rozsah 0-100
        if (percentage > 100.0f) percentage = 100.0f;
        if (percentage < 0.0f) percentage = 0.0f;

        RCLCPP_INFO(this->get_logger(),
                    "Received voltage: %.2f V -> battery: %.1f%%", voltage, percentage);

    	auto percent_msg = std_msgs::msg::Float32();
	percent_msg.data = percentage;
	
        // Tady zavoláš existující Publisher
        publisher_->publish(percent_msg);
        // protože Publisher už máš, stačí předat percent_msg
    }

    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr subscription_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr publisher_;
};
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<BatterySubscriber>());
  rclcpp::shutdown();
  return 0;
}
