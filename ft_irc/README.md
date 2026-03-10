*This project has been created as part of the 42 curriculum by rammisse, aboukhmi.*

Description :
    This project is about creating a functioning irc server in c++98. the goal is to nurture an understanding of networks programming and socket manipulation
    the server is designed to handle multiple clients and strictly adheres to a specific set of rules defined in the RFC manual.

Instructions :
    The project includes a makefile to automate the build process. to compile the server run: make

Execution :
    the compilation step will generate an executable which will be run using the following way: ./ircserv <port> <password>

Connecting to the server :
    to test if the server is working connect to it with nc from a seperate terminal : nc -C <localhost> <port>

Resources :
    RFC 1459 & RFC 2812: official documentation and protocol for the irc architecture
    geeksforgeeks guide for sockets in computer networks

AI usage:
    AI was used to clarify how exactly do Network programming functions work (socket, bind, accept, listen, recv, send)
    it was also used to get error codes 