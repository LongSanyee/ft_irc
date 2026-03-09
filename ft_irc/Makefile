NAME = ircserv

SRC = src/main.cpp src/parse.cpp src/Server.cpp src/Client.cpp \
	  src/Command.cpp src/Commands.cpp src/Channel.cpp

BONUS_SRC = bonus/src/main_bonus.cpp bonus/src/parse_bonus.cpp \
			bonus/src/Server_bonus.cpp bonus/src/Client_bonus.cpp \
			bonus/src/Command_bonus.cpp bonus/src/Commands_bonus.cpp \
			bonus/src/Channel_bonus.cpp

OBJ = $(SRC:.cpp=.o)

BONUS_OBJ = $(BONUS_SRC:.cpp=.o)

CFLAGS = -Wall -Wextra -Werror -std=c++98

C++ = c++

all: $(NAME)

$(NAME): $(OBJ)
		c++ $(CFLAGS) $(OBJ) -o $@

bonus: $(BONUS_OBJ)
		$(C++) $(CFLAGS) $(BONUS_OBJ) -o $(NAME)

%.o: %.cpp #TODO relink
		$(C++) $(CFLAGS) -c $< -o $@

clean:
		rm -rf $(OBJ) $(BONUS_OBJ)

fclean: clean
		rm -rf $(NAME)

re: fclean all