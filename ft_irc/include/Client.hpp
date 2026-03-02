#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <list>
#include <iostream>
#include <sstream> 
#include "Server.hpp"


class Client
{
public:
    Client(int _fd);
private:
    int fd;
    bool haspass;
    bool hasnick;
    bool hasinfo;
    bool isregistered;
    std::string nickname;
    std::string username;
    std::vector<std::string> invited;
};



#endif