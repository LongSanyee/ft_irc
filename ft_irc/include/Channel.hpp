#ifndef CHANNEL_HPP
#define CHANNEL_HPP

// #include "Client.hpp"
#include "irc.hpp"
#include "map"

class Channel
{
public:

private:
    std::string name;
    std::string topic;
    // std::map<std::string, Client &> clients;
    std::string pass;
    int limit;
    bool inviteonly;
    bool topicprotection;
};

#endif