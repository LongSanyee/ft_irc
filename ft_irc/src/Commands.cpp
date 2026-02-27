#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/irc.hpp"
#include "../include/Command.hpp"


void execute_pass(Command &cmd, Client &cl, Server &ser)
{
    if (cl.get_isregistred())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 462 * :You may not reregister\r\n");
        return ;
    }
    if (cmd.getparams().empty())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 461 * PASS :Not enough parameters\r\n");
        return ;
    }
    if (cmd.getparams()[0] != ser.get_passwd())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 464 * :Password incorrect\r\n");
        ser.disconnect_client(cl.get_fd());
        return ;
    }
    cl.set_hasPass(true);
}
void execute_cmd(Command &cmd, Client &cl, Server &ser)
{
    if (cmd.getcmd() == "PASS") {  }
    else if (cmd.getcmd() == "NICK") { }
    else if (cmd.getcmd() == "USER") {  }
    else if (cmd.getcmd() == "QUIT") {  }
    else if (cmd.getcmd() == "PING") {  }
    else if (cmd.getcmd() == "JOIN") {  }
    else if (cmd.getcmd() == "PRIVMSG") { }
    else { }

}



