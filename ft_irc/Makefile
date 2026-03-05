NAME = ircserv

SRC = src/main.cpp src/parse.cpp src/Server.cpp src/Client.cpp src/Command.cpp src/Commands.cpp src/Channel.cpp

BONUS = ../bonus/src/main_bonus.cpp ./bonus/src/parse_bonus.cpp ./bonus/src/Server_bonus.cpp ./bonus/src/Client_bonus.cpp ./bonus/src/Command_bonus.cpp \
		./bonus/src/Commands_bonus.cpp ./bonus/src/Channel_bonus.cpp

OBJ = $(SRC:.cpp=.o)

CFLAGS = -Wall -Wextra -Werror -fsanitize=address -g3

C++ = c++

all: $(NAME)

$(NAME): $(OBJ)
		$(C++) $(CFLAGS) $(OBJ) -o $@

%.o: %.c include/Client.hpp include/Server.hpp include/irc.hpp
		$(C++) $(CFLAGS) -c $< -o $@

clean:
		rm -rf $(OBJ)

fclean: clean
		rm -rf $(NAME)

re: fclean all