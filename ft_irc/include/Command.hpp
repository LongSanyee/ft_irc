#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "irc.hpp"


class Command
{
public:
    Command();
    Command(std::string extracted);
    void parsecmd(std::string &ex);
    std::string getcmd();
    std::vector<std::string> getparams();
private:
    std::string cmd;
    std::vector<std::string> params;
};



#endif