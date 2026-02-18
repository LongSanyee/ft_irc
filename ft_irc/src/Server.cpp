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
    if (server_fd < 0)
    {
        perror("socket error");
        std::exit(1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(6667);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind error");
        exit(1);
    }
    if (listen(server_fd, SOMAXCONN) < 0)
    {
        perror("listen error");
        exit(1);
    }
    fds.push_back({server_fd, POLLIN, 0});
}

void Server::eventloop()
{
    setsocket();
    while (1)
    {
        if (poll(fds.data(), fds.size(), 0) > 0)
        {
            int i = 0;
            while (i < fds.size())
            {
                if (fds[i].revents == POLLIN)
                {
                    if (fds[i].fd == server_fd)
                    {
                        accept()
                    }
                }
            }
        }
    }
}