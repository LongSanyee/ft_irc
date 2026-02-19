#ifndef IRC_HPP
#define IRC_HPP

#include <sstream>
#include <iostream>
#include <cctype>
#include "Server.hpp"
#include "Client.hpp"


int parseargs(char *port, char *pass);
std::vector<std::string> split(std::string str);

#endif