#include "../include/irc_bonus.hpp"

int main(int ac, char **av)
{
    if (ac != 3)
        return std::cout << "Usage: ./ircserv <port> <password>", 1;
    if (parseargs(av[1], av[2]))
        return 1;
    Server ser(av[1], av[2]);
    ser.eventloop();
}
