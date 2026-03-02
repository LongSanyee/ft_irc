#include "../include/irc.hpp"

int main(int ac, char **av)
{
    if (ac != 3)
        return std::cout << "Usage: ./ircserv <port> <password>", 1;
    if (parseargs(av[1], av[2]))
        return 1;
    Server ser(av[1], av[2]);
    ser.eventloop();
}




// if (cmd.getparams()[0].empty() || cmd.getparams()[0][0] != '#')
//     {
//         ser.sendmsg(cl.get_fd(), ":ircserv 403 " + cl.get_nickname() + " " + cmd.getparams()[0] + " :No such channel\r\n");
//         return;
//     }
//     std::map<std::string, Channel *>::iterator it = ser.getmap().find(cmd.getparams()[0]);
//     Channel *ch;
//     if (it != ser.getmap().end())
//     {
//         ch = it->second;
//         if (ch->getclients().find(cl.get_nickname()) != ch->getclients().end())
//             return;
//         if (ch->get_i() && cl.getclchannels()[cmd.getparams()[0]] != 1)
//         {
//             ser.sendmsg(cl.get_fd(), ":ircserv 473 " + cl.get_nickname() + " " + cmd.getparams()[0] + " :Cannot join channel (+i)\r\n");
//             return;
//         }
//         if (ch->get_k())
//         {
//             if (cmd.getparams().size() < 2 || cmd.getparams()[1] != ch->get_key())
//             {
//                 ser.sendmsg(cl.get_fd(), ":ircserv 475 " + cl.get_nickname() + " " + cmd.getparams()[0] + " :Cannot join channel (+k)\r\n");
//                 return;
//             }
//         }
//         if (ch->get_l())
//         {
//             if ((int)ch->getclients().size() >= ch->get_limit())
//             {
//                 ser.sendmsg(cl.get_fd(), ":ircserv 471 " + cl.get_nickname() + " " + cmd.getparams()[0] + " :Cannot join channel (+l)\r\n");
//                 return;
//             }
//         }
//         cl.getclchannels()[cmd.getparams()[0]] = 0;
//     }
//     else
//     {
//         ch = new Channel();
//         ch->setname(cmd.getparams()[0]);
//         ser.getmap()[cmd.getparams()[0]] =ch;
//         cl.getclchannels()[cmd.getparams()[0]] = 2;
//     }
//     ch->addclient(cl.get_nickname(), &cl);
//     ch->sendtoall(ser, ":" + cl.get_nickname() + "!" + cl.get_username() + "@" + cl.gethost() + " JOIN " + cmd.getparams()[0] + "\r\n");
//      if (ch->gettopic().empty())
//         ser.sendmsg(cl.get_fd(), ":ircserv 331 " + cl.get_nickname() + " " + cmd.getparams()[0] + " :No topic is set\r\n");
//     else
//         ser.sendmsg(cl.get_fd(), ":ircserv 332 " + cl.get_nickname() + " " + cmd.getparams()[0] + " :" + ch->gettopic() + "\r\n");
//     std::string names;
//     std::map<std::string, Client *>::iterator cit = ch->getclients().begin();
//     for (; cit != ch->getclients().end(); ++cit)
//     {
//         if (cit->second->getclchannels()[cmd.getparams()[0]] == 2)
//             names += "@" + cit->first + " ";
//         else
//             names += cit->first + " ";
//     }
//     ser.sendmsg(cl.get_fd(), ":ircserv 353 " + cl.get_nickname() + " = " + cmd.getparams()[0] + " :" + names + "\r\n");
//     ser.sendmsg(cl.get_fd(), ":ircserv 366 " + cl.get_nickname() + " " + cmd.getparams()[0] + " :End of /NAMES list\r\n");