#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <list>
#include <iostream>
#include <sstream> 
#include <vector>
#include <map>
#include "Server.hpp"


class Client
{
public:
    Client();
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
    void set_hasnick(bool n);
    void set_hasuser(bool u);
    void set_isregistered(bool i);
    void set_nickname(std::string nick);
    void set_username(std::string user);
    void sethostname(std::string host);
    std::string gethost();
    std::map<std::string, int>& getclchannels();
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
    std::map<std::string, int> _Channels;
};



#endif