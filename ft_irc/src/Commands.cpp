#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/irc.hpp"
#include "../include/Command.hpp"
#include <sstream>



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
    if (!cl.get_hasPass())
    {
        ser.sendmsg(cl.get_fd(), ":server 451 * :You have not registered");
        return;
    }
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
    if (!cl.get_hasNick() || (cl.get_hasNick() && cl.get_hasuser()))
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

void join_one(std::string first, std::string second, Client &cl, Server &ser)
{
    if (first.empty() || first[0] != '#')
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 403 " + cl.get_nickname() + " " + first + " :No such channel\r\n");
        return;
    }
    std::map<std::string, Channel *>::iterator it = ser.getmap().find(first);
    Channel *ch;
    if (it != ser.getmap().end())
    {
        ch = it->second;
        if (ch->getclients().find(cl.get_nickname()) != ch->getclients().end())
            return;
        if (ch->get_i() && cl.getclchannels()[first] != 1)
        {
            ser.sendmsg(cl.get_fd(), ":ircserv 473 " + cl.get_nickname() + " " + first + " :Cannot join channel (+i)\r\n");
            return;
        }
        if (ch->get_k())
        {
            if (second.empty() || second != ch->get_key())
            {
                ser.sendmsg(cl.get_fd(), ":ircserv 475 " + cl.get_nickname() + " " + first + " :Cannot join channel (+k)\r\n");
                return;
            }
        }
        if (ch->get_l())
        {
            if ((int)ch->getclients().size() >= ch->get_limit())
            {
                ser.sendmsg(cl.get_fd(), ":ircserv 471 " + cl.get_nickname() + " " + first + " :Cannot join channel (+l)\r\n");
                return;
            }
        }
        cl.getclchannels()[first] = 0;
    }
    else
    {
        ch = new Channel();
        ch->setname(first);
        ser.getmap()[first] =ch;
        cl.getclchannels()[first] = 2;
    }
    ch->addclient(cl.get_nickname(), &cl);
    ch->sendtoall(ser, ":" + cl.get_nickname() + "!" + cl.get_username() + "@" + cl.gethost() + " JOIN " + first + "\r\n");
     if (ch->gettopic().empty())
        ser.sendmsg(cl.get_fd(), ":ircserv 331 " + cl.get_nickname() + " " + first + " :No topic is set\r\n");
    else
        ser.sendmsg(cl.get_fd(), ":ircserv 332 " + cl.get_nickname() + " " + first + " :" + ch->gettopic() + "\r\n");
    std::string names;
    std::map<std::string, Client *>::iterator cit = ch->getclients().begin();
    for (; cit != ch->getclients().end(); ++cit)
    {
        if (cit->second->getclchannels()[first] == 2)
            names += "@" + cit->first + " ";
        else
            names += cit->first + " ";
    }
    ser.sendmsg(cl.get_fd(), ":ircserv 353 " + cl.get_nickname() + " = " + first + " :" + names + "\r\n");
    ser.sendmsg(cl.get_fd(), ":ircserv 366 " + cl.get_nickname() + " " + first + " :End of /NAMES list\r\n");
}

std::vector<std::string> split_cmd(std::string  str)
{
    std::stringstream first(str);
    std::string s;
    std::vector<std::string>tokens;
    while (std::getline(first, s, ','))
    {
        tokens.push_back(s);
    }
    return tokens;
}

void execute_join(Command &cmd, Client &cl, Server &ser)
{
    if (!cl.get_isregistred())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 451 " + cl.get_nickname() + " :You have not registered\r\n");
        return;
    }
    if (cmd.getparams().empty())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 461 " + cl.get_nickname() + " JOIN :Not enough parameters\r\n");
        return;
    }
    if (cmd.getparams()[0] == "0")
    {
        std::map<std::string,int>::iterator it = cl.getclchannels().begin();
        while (it != cl.getclchannels().end())
        {
            std::string chname = it->first;
            it++;
            Channel *ch = ser.getmap()[chname];
            ch->sendtoall(ser, ":" + cl.get_nickname() + "!" + cl.get_username() + "@" + cl.gethost() + " PART " + chname + "\r\n");
            ch->getclients().erase(cl.get_nickname());
            if (ch->getclients().empty())
            {
                delete ch;
                ser.getmap().erase(chname);
            }
        }
        cl.getclchannels().clear();
        return;
    }
    std::vector<std::string> ch_names = split_cmd(cmd.getparams()[0]);
    std::vector<std::string>ch_keys = split_cmd(cmd.getparams()[1]);
    int i = 0;
    while (!ch_names[i].empty())
    {
        join_one(ch_names[i], ch_keys[i], cl, ser);
        i++;
    }
    
}


void execute_quit(Command &cmd, Client &cl, Server &ser)
{
    std::string t = ":"+cl.get_nickname()+"!"+cl.get_username()+"@"+cl.gethost()+" QUIT :Client Quit\r\n";
    std::string s = ":"+cl.get_nickname()+"!"+cl.get_username()+"@"+cl.gethost()+" QUIT :"+cmd.getparams()[0]+"\r\n";
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

void execute_privmsg(Command &cmd, Client &cl, Server &ser)
{
    if (cmd.getparams().size() < 2)
    {
        if (cmd.getparams().size() == 1)
            ser.sendmsg(cl.get_fd(), ":ircserv 412 :No text to send\r\n");
        else
            ser.sendmsg(cl.get_fd(), ":ircserv 411 :No recipient given (PRIVMSG)\r\n");
        return ;
    }
    std::string chann = cmd.getparams()[0];
    std::string message = ":"+cl.get_nickname()+"!"+cl.get_username()+"@"+cl.gethost()+" PRIVMSG "+chann+" :"+cmd.getparams()[1];
    std::map<std::string, Channel *>::iterator it = ser.getmap().find(chann);
    Channel *tmp = it->second;
    if (tmp->getclients().find(cl.get_nickname()) == tmp->getclients().end())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 404 "+cl.get_nickname()+" "+chann+" :Cannot send to channel\r\n");
        return ;
    }
    if (it != ser.getmap().end())
    {
        Channel *tmp = it->second;
        std::map<std::string, Client *>::iterator iter = tmp->getclients().begin();
        for (;iter != tmp->getclients().end(); ++iter)
        {
            if (iter->second->get_fd() != cl.get_fd())
                ser.sendmsg(iter->second->get_fd(), message+"\r\n");
        }
    }
}
void execute_ping(Command &cmd, Client &cl, Server &ser)
{
    if (cmd.getparams().empty())
        return; 
    std::string pong = ":ircserv PONG ircserv :" + cmd.getparams()[0] + "\r\n";
    ser.sendmsg(cl.get_fd(), pong);
}

void execute_kick(Command &cmd, Client &cl, Server &ser)
{
    if (cl.)
}


void execute_cmd(Command &cmd, Client &cl, Server &ser)
{
    if (cmd.getcmd() == "PASS") {execute_pass(cmd,cl,ser);}
    else if (cmd.getcmd() == "NICK") {execute_nick(cmd,cl,ser);}
    else if (cmd.getcmd() == "USER") {  }
    else if (cmd.getcmd() == "QUIT") {execute_quit(cmd, cl, ser);}
    else if (cmd.getcmd() == "PING") {execute_ping(cmd, cl, ser);}
    else if (cmd.getcmd() == "JOIN") {execute_join(cmd, cl, ser); }
    else if (cmd.getcmd() == "PRIVMSG") {execute_privmsg(cmd,cl,ser);}
    else if (cmd.getcmd() == "KICK" ) {}
    else if (cmd.getcmd() == "INVITE" ){}
    else if (cmd.getcmd() == "TOPIC" ){}
    else if (cmd.getcmd() == "MODE") {}

}



