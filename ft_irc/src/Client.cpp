#include "../include/Client.hpp"


void Client::setdata(std::string str)
{
    Data += str;
}

std::string& Client::getdata()
{
    return this->Data;
}

bool Client::get_hasPass()
{
    return haspass;
};
bool Client::get_hasNick()
{
    return hasnick;
};
bool Client::get_hasuser()
{
    return hasuser;
};
bool Client::get_isregistred()
{
    return isregistered;
}

void Client::set_fd(int fd)
{
    this->fd = fd;
};

int Client::get_fd()
{
    return this->fd;
}

void Client::set_hasPass(bool t)
{
    this->haspass = t;
};
std::string Client::get_nickname()
{
    return this->nickname;
};

void Client::set_nickname(std::string nick)
{
    this->hasnick = true;
    this->nickname = nick;
};

std::string Client::get_username()
{
    return this->username;
};

void Client::sethostname(std::string host)
{
    hostname = host;
}

std::string Client::gethost()
{
    return this->hostname;
}

std::map<std::string, int>& Client::getclchannels()
{
    return _Channels;
}