#include "../include/Server.hpp"


Server::Server()
{

}

Server::~Server()
{

}

void Server::setsocket()
{
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(6667);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, SOMAXCONN);
    fds.push_back({server_fd, POLLIN, 0});
}