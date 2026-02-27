#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/irc.hpp"
#include "../include/Command.hpp"



bool valid_nick(std::string nm)
{
    std::string valid = "[]\\^_`{|}abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (nm.empty() || nm.length() > 9 || valid.find(nm[0]) == std::string::npos)
        return false;
    for (int i =0;i < nm.length();i++)
    {
        if (!isdigit(nm[i]) && valid.find(nm[i]) == std::string::npos && nm[i] != '-')
            return false;
    }
    return true;
}

void execute_nick(Command &cmd, Client &cl, Server &ser)
{
    if(!ser.verify_nick(cmd.getparams()[0]))
    {
        ser.sendmsg(cl.get_fd(), ":server 433 * <nick> :Nickname is already in use");
        return;
    }
    if (cmd.getparams()[0].empty())
    {
        ser.sendmsg(cl.get_fd(), ":server 431 * :No nickname given");
        return;
    }
    if (!valid_nick(cmd.getparams()[0]))
    {
        ser.sendmsg(cl.get_fd(), ":server 432 * <nick> :Erroneous nickname");
        return;
    }
    if (!cl.get_hasNick())
    {
        cl.set_nickname(cmd.getparams()[0]);
    }
    else
    {
        ser.broadcastmsg(":"+cl.get_nickname()+"!"+cl.get_username()+"@"+cl.gethost() + "NICK : "+cmd.getparams()[0]);
    }
}

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

void execute_quit(Command &cmd, Client &cl, Server &ser)
{
    std::string t = ":"+cl.get_nickname()+"!"+cl.get_username()+"@"+cl.gethost()+" QUIT :Client Quit";
    std::string s = ":"+cl.get_nickname()+"!"+cl.get_username()+"@"+cl.gethost()+" QUIT :"+cmd.getparams()[0];
    if (cl.get_isregistred())
    {
        std::vector<std::string> &tmp = cl.getclchannels();
        std::map<std::string, Channel*>::iterator it;
        for (int i = 0; i < tmp.size(); i++)
        {
            it = ser.getmap().find(tmp[i]);
            if (it != ser.getmap().end())
            {
                Channel *ch = it->second;
                if (cmd.getparams().empty())
                    ch->sendtoall(ser, t);
                else
                    ch->sendtoall(ser, s);
                ch->getclients().erase(cl.get_nickname());
                if (ch->getclients().empty())
                {
                    delete ch;
                    ser.getmap().erase(it);
                }
            }
        }
    }
    else
        ser.sendmsg(cl.get_fd(), "ERROR: Closing Link (User Quit)\r\n");
    ser.disconnect_client(cl.get_fd());
}

void execute_cmd(Command &cmd, Client &cl, Server &ser)
{
    if (cmd.getcmd() == "PASS") {execute_pass(cmd,cl,ser);}
    else if (cmd.getcmd() == "NICK") { }
    else if (cmd.getcmd() == "USER") {  }
    else if (cmd.getcmd() == "QUIT") {execute_quit(cmd, cl, ser);}
    else if (cmd.getcmd() == "PING") {  }
    else if (cmd.getcmd() == "JOIN") {  }
    else if (cmd.getcmd() == "PRIVMSG") { }
    else { }

}



