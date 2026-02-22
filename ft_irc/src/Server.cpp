#include "../include/Server.hpp"


Server::Server(char *pass, char *port)
{
	password = pass;
	std::stringstream t(port);
	t >> this->port;
}

Server::~Server()
{

}

void Server::sendmsg(int fd, std::string message)
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
	addr.sin_port = htons(this->port);
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

void Server::receivedata(int &i)
{
	char buff[1024] =  {0};
	int n = recv(fds[i].fd, buff, 1024, 0);
	if (n > 0)
	{
		std::string str(buff, n);
		std::string tmp = "\r\n";
		size_t pos = 0;
		while (pos != std::string::npos)
		{
			pos = str.find(tmp);
			if (pos != std::string::npos)
			{
				std::string extracted = str.substr(0, pos + tmp.size());
				str.erase(0, pos + tmp.size());
				Command cmd(extracted);
			}
		}
		i++;
	}
	else if (n == 0)
	{
		delete clients[fds[i].fd];
		clients.erase(fds[i].fd);
		close(fds[i].fd);
		std::swap(fds[i], fds[fds.size() - 1]);
		fds.pop_back();
	}
}

void Server::addclient(int &i)
{
    struct sockaddr_in temp;
	socklen_t t = sizeof(temp);
	int fd = accept(fds[i].fd, (sockaddr *)&temp, &t);
	if (fd >= 0)
	{
		struct pollfd n;
		n.fd = fd;
		n.events = POLLIN;
		n.revents = 0;
		fds.push_back(n);
		clients[fd] = new Client;
	}
	i++;
}

void Server::addclient()
{
    int i = 0;
    while (i < fds.size())
    {
        if (fds[i].revents == POLLIN)
        {
            if (fds[i].fd == server_fd)
				addclient(i);
			else
				receivedata(i);
        }
		else
			i++;
    }
}

void Server::eventloop()
{
    setsocket();
    while (1)
    {
        if (poll(fds.data(), fds.size(), -1) > 0)
        {
			addclient();
        }
    }
}
