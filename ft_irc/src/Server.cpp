#include "../include/Server.hpp"

bool Server::stop = false;

std::map<int, Client *> &Server::getclients()
{
	return clients;
}


void Server::signalhandler(int sig)
{
	(void)sig;
	Server::stop = true;
}

Server::Server(char *port, char *pass)
{
	password = pass;
	std::stringstream t(port);
	t >> this->port;
}

Server::~Server()
{
	std::map<int, Client *>::iterator it = clients.begin();
	while (it != clients.end())
	{
		close(it->first);
		delete it->second;
		it++;
	}
	// std::map<std::string, Channel *>::iterator iter = channels.begin();
	// for (;iter != channels.end(); iter++)
	// {
	// 	iter-
	// }
	close(server_fd);
	clients.clear();
	fds.clear();
}

void Server::sendmsg(int fd, std::string message)
{
	send(fd, message.c_str(), message.size(), MSG_NOSIGNAL);
}

void Server::broadcastmsg(std::string message)
{
	std::vector<pollfd>::iterator it = fds.begin();
	for (;it != fds.end(); it++)
	{
		sendmsg(it->fd, message);
	}
}

void Server::setsocket()
{
	this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
	{
		perror("socket error");
		std::exit(errno);
	}
	fcntl(server_fd, F_SETFL, O_NONBLOCK);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(this->port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		perror("bind error");
		exit(errno);
	}
	if (listen(server_fd, SOMAXCONN) < 0)
	{
		perror("listen error");
		exit(errno);
	}
	struct pollfd tmp;
	tmp.fd = server_fd;
	tmp.events = POLLIN;
	tmp.revents = 0;
	fds.push_back(tmp);
}

void Server::disconnect_client(int fd)
{
    for (size_t i = 0; i < fds.size(); i++)
    {
        if (fds[i].fd == fd)
        {
            std::swap(fds[i], fds[fds.size() - 1]);
            fds.pop_back();
            break ;
        }
    }
    if (clients.count(fd))
    {
        delete clients[fd];
        clients.erase(fd);
    }
    close(fd);
}

void Server::receivedata(int &i)
{
	char buff[1024] =  {0};
	int n = recv(fds[i].fd, buff, 1024, 0);
	if (n > 0)
	{
        clients[fds[i].fd]->getdata().append(buff, n);
        std::string &data = clients[fds[i].fd]->getdata();
        std::string delim = "\r\n";
        size_t pos;
        while ((pos = data.find(delim)) != std::string::npos)
        {
            std::string line = data.substr(0, pos);
            data.erase(0, pos + delim.size());
            if (line.empty())
                continue;
            Command cmd(line);
            execute_cmd(cmd, *clients[fds[i].fd], *this);
        }
        i++;
	}
	else if (n == 0)
	{
		disconnect_client(fds[i].fd);
	}
}

void Server::addclient(int &i)
{
    struct sockaddr_in temp;
	socklen_t t = sizeof(temp);
	int fd = accept(fds[i].fd, (sockaddr *)&temp, &t);
	char *addr = inet_ntoa(temp.sin_addr);
	if (fd >= 0)
	{
		fcntl(fd, F_SETFL, O_NONBLOCK);
		struct pollfd n;
		n.fd = fd;
		n.events = POLLIN;
		n.revents = 0;
		fds.push_back(n);
		clients[fd] = new Client;
		clients[fd]->set_fd(fd);
		clients[fd]->sethostname(addr);
	}
	i++;
}

void Server::addclient()
{
    int i = 0;
    while (i < (int)fds.size())
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
	struct sigaction sa;
	sa.sa_handler = signalhandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
    setsocket();
    while (1)
    {
        if (poll(fds.data(), fds.size(), -1) > 0)
        {
			addclient();
        }
    }
}

std::string Server::get_passwd()
{
	return this->password;
}

bool Server::verify_nick(std::string nick)
{
	std::map<int, Client *>::iterator it = clients.begin();
	for (;it != clients.end(); ++it)
	{
		if (it->second->get_nickname() == nick)
			return false;
	}
	return true;
};

std::map<std::string, Channel *>& Server::getmap()
{
	return channels;
}

Client* Server::getclientbynick(const std::string &nickname)
{
    std::map<int, Client *>::iterator it = clients.begin();
    for (; it != clients.end(); ++it)
    {
        if (it->second->get_nickname() == nickname)
            return it->second;
    }
    return NULL;
}