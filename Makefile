NAME = webserv

CPPFLAGS = -I include/

CXX = c++

CPPFLAGS = -I include/ -D DEF_ROOT=$(DEFAULT_ROOT)#-D DEBUG
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -O0 -g3 -Wshadow -Wno-shadow -pedantic -fsanitize=address,leak

DEFAULT_ROOT = \"$(shell pwd)/error_pages\"

OBJ = $(SRC:.cpp=.o)

DEFAULT_ROOT = \"$(shell pwd)/error_pages\"

SRC = \
	src/main.cpp\
	src/socket_management.cpp\
	src/get_config_data.cpp\
	src/ARequest.cpp\
	src/BADRequest.cpp\
	src/GETRequest.cpp\
	src/POSTRequest.cpp\
	src/DELETERequest.cpp\
	src/Listener.cpp\
	src/Location.cpp\
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
