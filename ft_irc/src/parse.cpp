#include "../include/irc.hpp"


std::vector<std::string> split(std::string str)
{
    std::vector<std::string> r;
    std::stringstream t(str);
    std::string words;

    while ((t >> words))
        r.push_back(words);
    return r;
}

int invalid(std::string t)
{
    int i = 0;
    for (;i < t.size(); i++)
    {
        if (isspace(t[i]) || !isprint(t[i]))
            return 1;
    }
    return 0;
}

int parsepass(char *pass)
{
    std::string str = pass;

    if (str.empty() || invalid(str))
        return std::cout << "Invalid password !", 1;
    return 0;
}

int parseport(char *port)
{
    int num = 0;
    char extra;
    std::stringstream ss(port);

    if (!(ss >> num) || (ss >> extra))
        return std::cout << "Invalid Port !", 1;
    else if (num < 0 || num > 65535)
        return std::cout << "Port not within range !", 1;
    return 0;
}

int parseargs(char *port, char *pass)
{
    if (parsepass(pass) || parseport(port))
        return 1;
    return 0;
}

