#ifndef SERVER_HPP
#define SERVER_HPP

#include "Channel.hpp"
#include <fcntl.h>
#include <vector>
#include "Command.hpp"
#include <sys/socket.h>
#include <poll.h>
#include <arpa/inet.h>
#include "Client.hpp"
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <map>
#include <iostream>
#include <algorithm>

class Client;
class Channel;

class Server
{
public:
    Server(char *port, char *pass);
    ~Server();
    void setsocket();
    void eventloop();
    void addclient();
    void sendmsg(int fd, std::string message);
    void broadcastmsg(std::string message);
    void receivedata(int &i);
    void addclient(int &i);
    std::string get_passwd();
    void disconnect_client(int fd);
    bool verify_nick(std::string nick);
    Client*  getclientbynick(const std::string &nickname);
    std::map<std::string, Channel *> &getmap();
    std::map<int, Client *> &getclients();
    static void signalhandler(int sig);
private:
    std::vector<struct pollfd> fds;
    std::map<int, Client*> clients;
    std::map<std::string, Channel *> channels;
    int server_fd;
    int port;
    std::string password;
    static bool stop;
};

void execute_cmd(Command &cmd, Client &cl, Server &ser);
#endif