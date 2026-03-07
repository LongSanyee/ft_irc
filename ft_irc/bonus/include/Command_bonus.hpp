#ifndef COMMAND_BONUS_HPP
#define COMMAND_BONUS_HPP

#define ERR_NEEDMOREPARAMS 461
#define ERR_PASSWDMISMATCH 464
#define ERR_ALREADYREGISTRED 462

#include "irc_bonus.hpp"


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