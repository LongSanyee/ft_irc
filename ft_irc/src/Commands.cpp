#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/irc.hpp"
#include "../include/Command.hpp"


void execute_cmd(Command &cmd, Client &cl)
{
    if (cmd.getcmd() == "PASS") {  }
    else if (cmd.getcmd() == "NICK") { }
    else if (cmd.getcmd() == "USER") {  }
    else if (cmd.getcmd() == "QUIT") {  }
    else if (cmd.getcmd() == "PING") {  }
    else if (cmd.getcmd() == "JOIN") {  }
    else if (cmd.getcmd() == "PRIVMSG") { }
    else {  }

}