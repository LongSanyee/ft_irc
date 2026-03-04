#include "../include/Channel.hpp"


void Channel::setname(std::string _name)
{
    name = _name;
}

void Channel::settopic(std::string _topic)
{
    topic = _topic;
}

std::string Channel::gettopic()
{
    return topic;
}

std::string Channel::getname()
{
    return name;
}

void Channel::addclient(std::string nick, Client *ref)
{
    clients[nick] = ref;
}

void Channel::sendtoall(Server &ser, std::string message)
{
    std::map<std::string, Client *>::iterator it;
    for (it = this->clients.begin(); it != clients.end(); ++it)
    {
        ser.sendmsg(it->second->get_fd(), message);
    }
}

std::map<std::string, Client *>& Channel::getclients()
{
    return clients;
}
bool Channel::get_i()
{
    return this->i;
};
bool Channel::get_k()
{
    return this->k;
};
bool Channel::get_l()
{
    return this->l;
};
std::string Channel::get_key()
{
    return this->key;
};
int Channel::get_limit()
{
    return this->limit;
};
bool Channel::get_t()
{
    
};