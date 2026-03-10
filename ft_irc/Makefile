NAME = ircserv

SRC = src/main.cpp src/parse.cpp src/Server.cpp src/Client.cpp \
	  src/Command.cpp src/Commands.cpp src/Channel.cpp

OBJ = $(SRC:.cpp=.o)

CFLAGS = -Wall -Wextra -Werror -std=c++98

C++ = c++

all: $(NAME)

$(NAME): $(OBJ)
		c++ $(CFLAGS) $(OBJ) -o $@

%.o: %.cpp #TODO relink
		$(C++) $(CFLAGS) -c $< -o $@

clean:
		rm -rf $(OBJ) $(BONUS_OBJ)

fclean: clean
		rm -rf $(NAME)

re: fclean all