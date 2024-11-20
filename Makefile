NAME = webserv

CPPFLAGS = -I include/

CXX = c++

CPPFLAGS = -I include/
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -O0 -g3 -Wshadow -Wno-shadow -pedantic -fsanitize=address,leak

OBJ = $(SRC:.cpp=.o)

SRC = \
	src/main.cpp\
	src/socket_management.cpp\
	src/get_config_data.cpp\
	src/Listener.cpp\
	src/Server.cpp

#-------------------------------------------------------------------------------

all: $(NAME)

re: fclean all

$(NAME): $(OBJ)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)

.PHONY: all clean fclean re
.SECONDARY: $(OBJ)
