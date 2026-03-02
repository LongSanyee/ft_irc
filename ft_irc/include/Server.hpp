#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include "Command.hpp"
#include "Channel.hpp"
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Client.hpp"
#include <errno.h>
#include <stdio.h>
#include <cstdlib>
#include <map>
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <algorithm>

class Client;

class Server
{
public:
    Server(char *pass, char *port);
    ~Server();
    void setsocket();
    void eventloop();
    void addclient();
    void sendmsg(int fd, std::string message);
    void receivedata(int &i);
    void addclient(int &i);
    static void signalhandler(int sig);
private:
    std::vector<struct pollfd> fds;
    std::map<int, Client*> clients;
    std::map<std::string, Channel *> channels;
    std::string password;
    int server_fd;
    int port;
    static bool stop;
};


#endif