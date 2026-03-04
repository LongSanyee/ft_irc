#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include "Client.hpp"
#include "Server.hpp"
#include "irc.hpp"

#define INVITE 1
#define OPERATOR 2
#define REGULAR 0

class Server;
class Client;
class Command;

class Channel
{
public:
    void setname(std::string _name);
    void settopic(std::string _topic);
    void addclient(std::string nick, Client *ref);
    std::string gettopic();
    std::string getname();
    void sendtoall(Server &ser, std::string message);
    std::map<std::string, Client *>& getclients();
    bool get_i();
    bool get_k();
    bool get_l();
    bool get_t();
    std::string get_key();
    int get_limit();
    void set_i(bool val);
    void set_t(bool val);
    void set_k(bool val);
    void set_key(std::string key);
    void set_l(bool val);
    void set_limit(int lim);
private:
    std::string name;
    bool i;
    bool t;
    bool k;
    bool o;
    bool l;
    std::string topic;
    std::string key;
    int limit;
    std::map<std::string, Client *> clients;
};

#endif