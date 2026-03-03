#include <mpc-rbt-solution/Sender.hpp>

void Sender::Node::run()
{
  while (errno != EINTR) {
    if ((std::chrono::steady_clock::now() - timer_tick) < timer_period) continue;
    timer_tick = std::chrono::steady_clock::now();

    callback();
  }
}

void Sender::Node::onDataTimerTick()
{
    data.x += 1.0;
    data.y += 1.0;
    data.z += 1.0;
    data.timestamp = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());

  
    Socket::IPFrame frame;
    frame.port = config.remotePort;
    frame.address = config.remoteAddress;

  
    if(!Utils::Message::serialize(frame, data))
    {
        RCLCPP_ERROR(logger, "Serialization failed");
        return;
    }

    
    if(send(frame))
    {
        RCLCPP_INFO(logger, "Sent: x=%f y=%f z=%f", data.x, data.y, data.z);
    }
    else
    {
        RCLCPP_ERROR(logger, "Send failed");
    }

  
    RCLCPP_INFO(logger, "Sending data to host: '%s:%d'", frame.address.c_str(), frame.port);
    RCLCPP_INFO(logger, "\n\tstamp: %ld", data.timestamp);
}
