NAME = ircserv

SRC = src/main.cpp src/parse.cpp src/Server.cpp src/Client.cpp

OBJ = $(SRC:.cpp=.o)

CFLAGS = -Wall -Wextra -Werror

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