#ifndef __RS232_H_INCLUDED___
#define __RS232_H_INCLUDED___

#include <asio.hpp>
#include <string>

class RS232
{
public:
    RS232(std::string,int);
    void writeString(std::string);
    std::string readLine();
private:
    asio::io_service io;
    asio::serial_port serial;
}
#endif
