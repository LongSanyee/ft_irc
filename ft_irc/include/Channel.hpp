#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include "irc.hpp"


class Channel
{
public:
    void setname(std::string _name);
    void settopic(std::string _topic);
    void addclient(std::string nick, Client *ref);
    std::string gettopic();
    std::string getname();
    void sendtoall(Server &ser, std::string message);
private:
    std::string name;
    std::string topic;
    std::map<std::string, Client *> clients;
};

#endif