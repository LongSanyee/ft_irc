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
    if (cl.get_isregistred())
    {
        cl.set_nickname(cmd.getparams()[0]);
        return ;
    }
    cl.set_hasnick(true);
    cl.set_nickname(cmd.getparams()[0]);
    if (cl.get_hasuser())
    {
        cl.set_isregistered(true);
        std::string tmp = cl.get_nickname()+"!"+cl.get_username()+"@"+cl.gethost();
        ser.sendmsg(cl.get_fd(), ":ircserv 001 "+cl.get_nickname()+" :Welcome to the Internet Relay Chat Network "+tmp);
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
        std::map<std::string, int>::iterator inv = cl.getclchannels().find(first);
        if (ch->get_i() && (inv == cl.getclchannels().end() || inv->second != 1))
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
        std::map<std::string, int>::iterator role = cit->second->getclchannels().find(first);
        if (role != cit->second->getclchannels().end() && role->second == 2)
            names += "@" + cit->first + " ";
        else
            names += cit->first + " ";
    }
    ser.sendmsg(cl.get_fd(), ":ircserv 353 " + cl.get_nickname() + " = " + first + " :" + names + "\r\n");
    ser.sendmsg(cl.get_fd(), ":ircserv 366 " + cl.get_nickname() + " " + first + " :End of /NAMES list\r\n");
}

std::vector<std::string> split_cmd(std::string str)
{
    std::stringstream first(str);
    std::string s;
    std::vector<std::string> tokens;
    while (std::getline(first, s, ','))
    {
        size_t start = s.find_first_not_of(' ');
        if (start != std::string::npos)
            tokens.push_back(s.substr(start));
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
    std::vector<std::string>ch_keys;
    if (cmd.getparams().size() >= 2 && !cmd.getparams()[1].empty())
        ch_keys = split_cmd(cmd.getparams()[1]);
    int i = 0;
    while (i < (int)ch_names.size())
    {
        std::string key = (i < (int)ch_keys.size()) ? ch_keys[i] : "";
        join_one(ch_names[i], key, cl, ser);
        i++;
    }
    
}


void execute_quit(Command &cmd, Client &cl, Server &ser)
{
    std::string t = ":"+cl.get_nickname()+"!"+cl.get_username()+"@"+cl.gethost()+" QUIT :Client Quit\r\n";
    std::string s = ":"+cl.get_nickname()+"!"+cl.get_username()+"@"+cl.gethost()+" QUIT :";
    if (cl.get_isregistred())
    {
        std::map<std::string, int> &tmp = cl.getclchannels();
        std::map<std::string, int>::iterator iter = tmp.begin();
        std::map<std::string, Channel*>::iterator it;
        for (; iter != tmp.end(); ++iter)
        {
            it = ser.getmap().find(iter->first);
            if (it != ser.getmap().end())
            {
                Channel *ch = it->second;
                if (cmd.getparams().empty())
                    ch->sendtoall(ser, t);
                else
                    ch->sendtoall(ser, s+cmd.getparams()[0]+"\r\n");
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
    if (!cl.get_isregistred())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 451 " + cl.get_nickname() + " :You have not registered\r\n");
        return;
    }
    if (cmd.getparams().size() < 2)
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 461 " + cl.get_nickname() + " KICK :Not enough parameters\r\n");
        return;
    }
    std::string chname = cmd.getparams()[0];
    std::string target = cmd.getparams()[1];
    std::string reason = (cmd.getparams().size() >= 3) ? cmd.getparams()[2] : cl.get_nickname();
    std::map<std::string, Channel *>::iterator it = ser.getmap().find(chname);
    if (it == ser.getmap().end())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 403 " + cl.get_nickname() + " " + chname + " :No such channel\r\n");
        return;
    }
    Channel *ch = it->second;
    if (ch->getclients().find(cl.get_nickname()) == ch->getclients().end())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 442 " + cl.get_nickname() + " " + chname + " :You're not on that channel\r\n");
        return;
    }
    std::map<std::string, int>::iterator role = cl.getclchannels().find(chname);
    if (role == cl.getclchannels().end() || role->second != 2)
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 482 " + cl.get_nickname() + " " + chname + " :You're not channel operator\r\n");
        return;
    }
     std::map<std::string, Client *>::iterator target_it = ch->getclients().find(target);
    if (target_it == ch->getclients().end())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 441 " + cl.get_nickname() + " " + target + " " + chname + " :They aren't on that channel\r\n");
        return;
    }
    ch->sendtoall(ser, ":" + cl.get_nickname() + "!" + cl.get_username() + "@" + cl.gethost()
        + " KICK " + chname + " " + target + " :" + reason + "\r\n");
    Client *kicked = target_it->second;
    ch->getclients().erase(target);
    kicked->getclchannels().erase(chname);
    if (ch->getclients().empty())
    {
        delete ch;
        ser.getmap().erase(chname);
    }
}

void execute_invite(Command &cmd, Client &cl, Server &ser)
{
    if (!cl.get_isregistred())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 451 " + cl.get_nickname() + " :You have not registered\r\n");
        return;
    }
    if (cmd.getparams().size() < 2)
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 461 " + cl.get_nickname() + " INVITE :Not enough parameters\r\n");
        return;
    }
    std::string target = cmd.getparams()[0];
    std::string chname = cmd.getparams()[1];
    Client *target_cl = ser.getclientbynick(target);
    if (!target_cl)
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 401 " + cl.get_nickname() + " " + target + " :No such nick/channel\r\n");
        return;
    }
     std::map<std::string, Channel *>::iterator it = ser.getmap().find(chname);
    if (it == ser.getmap().end())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 403 " + cl.get_nickname() + " " + chname + " :No such channel\r\n");
        return;
    }
    Channel *ch = it->second;
    if (ch->getclients().find(cl.get_nickname()) == ch->getclients().end())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 442 " + cl.get_nickname() + " " + chname + " :You're not on that channel\r\n");
        return;
    }
    if (ch->get_i())
    {
        std::map<std::string, int>::iterator role = cl.getclchannels().find(chname);
        if (role == cl.getclchannels().end() || role->second != 2)
        {
            ser.sendmsg(cl.get_fd(), ":ircserv 482 " + cl.get_nickname() + " " + chname + " :You're not channel operator\r\n");
            return;
        }
    }
    if (ch->getclients().find(target) != ch->getclients().end())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 443 " + cl.get_nickname() + " " + target + " " + chname + " :is already on channel\r\n");
        return;
    }
    target_cl->getclchannels()[chname] = 1;
    ser.sendmsg(cl.get_fd(), ":ircserv 341 " + cl.get_nickname() + " " + target + " " + chname + "\r\n");
    ser.sendmsg(target_cl->get_fd(), ":" + cl.get_nickname() + "!" + cl.get_username() + "@" + cl.gethost()+ " INVITE " + target + " " + chname + "\r\n");
}

void mode_query(Channel *ch, Client &cl, Server &ser, const std::string &chname)
{
    std::string modes = "+";
    std::string params = "";

    if (ch->get_i()) modes += "i";
    if (ch->get_t()) modes += "t";
    if (ch->get_k())
    {
        modes += "k";
        params += " " + ch->get_key();
    }
    if (ch->get_l())
    {
        modes += "l";
        std::stringstream ss;
        ss << ch->get_limit();
        params += " " + ss.str();
    }
    if (modes == "+")
        modes = "";
    ser.sendmsg(cl.get_fd(), ":ircserv 324 " + cl.get_nickname() + " " + chname + " " + modes + params + "\r\n");
}

void execute_mode(Command &cmd, Client &cl, Server &ser)
{
    if (!cl.get_isregistred())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 451 " + cl.get_nickname() + " :You have not registered\r\n");
        return;
    }
     if (cmd.getparams().empty())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 461 " + cl.get_nickname() + " MODE :Not enough parameters\r\n");
        return;
    }
    std::string chname = cmd.getparams()[0];
     std::map<std::string, Channel *>::iterator it = ser.getmap().find(chname);
    if (it == ser.getmap().end())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 403 " + cl.get_nickname() + " " + chname + " :No such channel\r\n");
        return;
    }
    Channel *ch = it->second;
    if (cmd.getparams().size() == 1)
    {
        mode_query(ch, cl, ser, chname);
        return;
    }
    if (ch->getclients().find(cl.get_nickname()) == ch->getclients().end())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 442 " + cl.get_nickname() + " " + chname + " :You're not on that channel\r\n");
        return;
    }
    std::map<std::string, int>::iterator role = cl.getclchannels().find(chname);
    if (role == cl.getclchannels().end() || role->second != 2)
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 482 " + cl.get_nickname() + " " + chname + " :You're not channel operator\r\n");
        return;
    }
    std::string modestring = cmd.getparams()[1];
    int arg_index = 2;
    bool adding = true;
    std::string applied_modes = "";
    std::string applied_params = "";
    char last_sign = 0;
    for (size_t i = 0; i < modestring.size(); i++)
    {
        char c = modestring[i];

        if (c == '+')
        {
            adding = true;
            continue;
        }
        if (c == '-')
        {
            adding = false;
            continue;
        }
        if (c == 'i')
        {
            if (adding && !ch->get_i())
            {
                ch->set_i(true);
                if (last_sign != '+') { applied_modes += "+"; last_sign = '+'; }
                applied_modes += "i";
            }
            else if (!adding && ch->get_i())
            {
                ch->set_i(false);
                if (last_sign != '-') { applied_modes += "-"; last_sign = '-'; }
                applied_modes += "i";
            }
        }
        else if (c == 't')
        {
            if (adding && !ch->get_t())
            {
                ch->set_t(true);
                if (last_sign != '+') { applied_modes += "+"; last_sign = '+'; }
                applied_modes += "t";
            }
            else if (!adding && ch->get_t())
            {
                ch->set_t(false);
                if (last_sign != '-') { applied_modes += "-"; last_sign = '-'; }
                applied_modes += "t";
            }
        }
        else if (c == 'k')
        {
            if (adding)
            {
                if (arg_index >= (int)cmd.getparams().size())
                {
                    ser.sendmsg(cl.get_fd(), ":ircserv 461 " + cl.get_nickname() + " MODE :Not enough parameters\r\n");
                    continue;
                }
                std::string key = cmd.getparams()[arg_index++];
                ch->set_k(true);
                ch->set_key(key);
                if (last_sign != '+') { applied_modes += "+"; last_sign = '+'; }
                applied_modes += "k";
                applied_params += " " + key;
            }
            else
            {
                ch->set_k(false);
                ch->set_key("");
                if (last_sign != '-') { applied_modes += "-"; last_sign = '-'; }
                applied_modes += "k";
            }
        }
        else if (c == 'l')
        {
            if (adding)
            {
                if (arg_index >= (int)cmd.getparams().size())
                {
                    ser.sendmsg(cl.get_fd(), ":ircserv 461 " + cl.get_nickname() + " MODE :Not enough parameters\r\n");
                    continue;
                }
                std::string lim_str = cmd.getparams()[arg_index++];
                std::stringstream ss(lim_str);
                int lim = 0;
                ss >> lim;
                if (lim <= 0)
                    continue;
                ch->set_l(true);
                ch->set_limit(lim);
                if (last_sign != '+') { applied_modes += "+"; last_sign = '+'; }
                applied_modes += "l";
                applied_params += " " + lim_str;
            }
            else
            {
                ch->set_l(false);
                ch->set_limit(0);
                if (last_sign != '-') { applied_modes += "-"; last_sign = '-'; }
                applied_modes += "l";
            }
        }
        else if (c == 'o')
        {
            if (arg_index >= (int)cmd.getparams().size())
            {
                ser.sendmsg(cl.get_fd(), ":ircserv 461 " + cl.get_nickname() + " MODE :Not enough parameters\r\n");
                continue;
            }
            std::string nick = cmd.getparams()[arg_index++];
            Client *target = ser.getclientbynick(nick);
            if (!target)
            {
                ser.sendmsg(cl.get_fd(), ":ircserv 401 " + cl.get_nickname() + " " + nick + " :No such nick/channel\r\n");
                continue;
            }
            if (ch->getclients().find(nick) == ch->getclients().end())
            {
                ser.sendmsg(cl.get_fd(), ":ircserv 441 " + cl.get_nickname() + " " + nick + " " + chname + " :They aren't on that channel\r\n");
                continue;
            }

            if (adding)
            {
                target->getclchannels()[chname] = 2;
                if (last_sign != '+') { applied_modes += "+"; last_sign = '+'; }
                applied_modes += "o";
                applied_params += " " + nick;
            }
            else
            {
                target->getclchannels()[chname] = 0;
                if (last_sign != '-') { applied_modes += "-"; last_sign = '-'; }
                applied_modes += "o";
                applied_params += " " + nick;
            }
        }
        else
        {
            ser.sendmsg(cl.get_fd(), ":ircserv 472 " + cl.get_nickname() + " " + std::string(1, c) + " :is unknown mode char to me\r\n");
        }
        if (!applied_modes.empty())
        {
            ch->sendtoall(ser, ":" + cl.get_nickname() + "!" + cl.get_username() + "@" + cl.gethost()+ " MODE " + chname + " " + applied_modes + applied_params + "\r\n");
        }
    }

}

void execute_user(Command &cmd, Client &cl, Server &ser)
{
    if (!cl.get_hasPass())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 451 * :You have not registered\r\n");
        return;
    }
    if (cl.get_isregistred())
    {
        ser.sendmsg(cl.get_fd(), ":ircserv 462 "+cl.get_nickname()+":You may not register\r\n");
        return;
    }
    if (cmd.getparams().size() < 4)
    {
        ser.sendmsg(cl.get_fd(), ":servername 461 "+cl.get_nickname()+" :Not enough parameters\r\n");
        return ;
    }
    cl.set_username(cmd.getparams()[0]);
    cl.set_hasuser(true);
    if (cl.get_hasNick())
    {
        cl.set_isregistered(true);
        std::string tmp = cl.get_nickname()+"!"+cl.get_username()+"@"+cl.gethost();
        ser.sendmsg(cl.get_fd(), ":ircserv 001 "+cl.get_nickname()+" :Welcome to the Internet Relay Chat Network "+tmp);
    }
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
    else if (cmd.getcmd() == "KICK" ) {execute_kick(cmd, cl, ser);}
    else if (cmd.getcmd() == "INVITE" ){execute_invite(cmd, cl, ser);}
    else if (cmd.getcmd() == "TOPIC" ){}
    else if (cmd.getcmd() == "MODE") {}

}



