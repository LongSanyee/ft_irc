#include "../include/Client.hpp"


void Client::setdata(std::string str)
{
    Data += str;
}

std::string& Client::getdata()
{
    return this->Data;
}

