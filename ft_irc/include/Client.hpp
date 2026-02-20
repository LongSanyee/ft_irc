#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <list>
#include <iostream>
#include <sstream> 
#include "Server.hpp"


class Client
{
public:
    void setdata(std::string str);
private:
    bool haspass;
    bool hasnick;
    bool hasinfo;
    bool isregistered;
    std::string Data;
    std::string nickname;
    std::string username;
    std::list<std::string> channels;
};



#endif