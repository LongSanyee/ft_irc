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

class Server
{
public:
    Server();
    ~Server();
    void setsocket();
    void eventloop();
private:
    std::vector<struct pollfd> fds;
    std::map<int, Client*> clients;
    int server_fd;
};



#endif