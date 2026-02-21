#include "../include/Command.hpp"

Command::Command()
{

}

std::vector<std::string> split(std::string &str)
{
    std::vector<std::string> vec;
    size_t pos = 0;

    pos = str.find(" :");
    if (pos != std::string::npos)
    {
        std::string tmp;
        if (pos + 2 >= str.size())
            tmp = "";
        else
            tmp = str.substr(pos + 2, std::string::npos);
        str.erase(pos, std::string::npos);
        std::stringstream t(str);
        std::string words;
        while (t >> words)
            vec.push_back(words);
        vec.push_back(tmp);
    }
    else
    {
        std::stringstream tmp(str);
        std::string words;
        while (tmp >> words)
            vec.push_back(words);
    }
    return vec;
}

void Command::parsecmd(std::string& ex)
{
    int k = 0;
    while (ex[k] && isspace(ex[k]))
        k++;
    ex.erase(0, k);
    size_t pos = ex.find(" ");
    if (pos != std::string::npos)
    {
        cmd = ex.substr(0, pos);
        ex.erase(0, pos);
    }
    else if (pos == std::string::npos)
    {
        cmd = ex;
        ex.clear();
    }
    params = split(ex);
}

Command::Command(std::string extracted)
{
    parsecmd(extracted);
}

std::string Command::getcmd()
{
    return this->cmd;
}

std::vector<std::string> Command::getparams()
{
    return this->params;
}
