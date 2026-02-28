#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <list>
#include <iostream>
#include <sstream> 
#include <vector>
#include "Server.hpp"


class Client
{
public:
    void setdata(std::string str);
    std::string& getdata();
    bool get_hasPass();
    bool get_hasNick();
    bool get_hasuser();
    bool get_isregistred();
    int  get_fd();
    std::string get_nickname();
    std::string get_username();
    void set_fd(int fd);
    void set_hasPass(bool t);
    void set_nickname(std::string nick);
    void sethostname(std::string host);
    std::string gethost();
    std::vector<std::string>& getclchannels();
private:
    int fd;
    bool haspass;
    bool hasnick;
    bool hasuser;
    bool isregistered;
    std::string Data;
    std::string nickname;
    std::string username;
    std::string hostname;
    std::vector<std::string> _Channels;
};



#endif