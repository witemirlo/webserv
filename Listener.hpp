#ifndef LISTENER_HPP
# define LISTENER_HPP

# include <poll.h>
# include <vector>

# include "Server.hpp"

class Listener
{
private:
	struct pollfd _listener;
	std::vector<struct pollfd> _derived_socks;
	std::vector<Server> _assoc_servers;
public:

//	OCCF
	Listener();
	Listener(const Listener &other);
	Listener &operator=(const Listener &other);
	~Listener();
};

# ifndef COLOR_DEF
#  define COLOR_DEF
#  define NC "[0m"
#  define GREEN "[38;5;40m"
#  define YELOW "[38;5;220m"
#  define RED "[38;5;124m"
#  define BLUE "[38;5;32m"
#  define ORAN "[38;5;202m"
#  define PURP "[38;5;91m"
# endif

#endif
