#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Server
{
public:
    Server();
    ~Server();
    void setsocket();
private:
    std::vector<struct pollfd> fds;
    int server_fd;
};



#endif