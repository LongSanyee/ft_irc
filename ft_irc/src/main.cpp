#include "../include/irc.hpp"

int main(int ac, char **av)
{
    if (ac != 3)
        return std::cout << "Usage: ./ircserv <password> <port>", 1;
    if (parseargs(av[1], av[2]))
        return 1;
}