#include <RS232.hpp>

RS232::RS232(std::string port_name, int baudrate)
    : io()
    , serial(io,port)
{
    serial.set_option(asio::serial_port_base::baud_rate(baudrate));
} 

