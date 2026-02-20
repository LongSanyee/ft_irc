#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/Client.hpp"
#include <errno.h>
#include <stdio.h>
#include <cstdlib>
#include <map>
#include <iostream>
#include <unistd.h>
#include <algorithm>

class Server
{
public:
    Server(char *pass, char *port);
    ~Server();
    void setsocket();
    void eventloop();
    void addclient();
private:
    std::vector<struct pollfd> fds;
    std::map<int, Client*> clients;
    int server_fd;
    int port;
    std::string password;
};



#endif