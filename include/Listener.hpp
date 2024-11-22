#ifndef LISTENER_HPP
# define LISTENER_HPP

# include <poll.h>
# include <string>
# include <vector>

# include "colors.hpp"
# include "Server.hpp"
# include "ARequest.hpp"

# define FD_IS_LISTENER -1
# define FD_NOT_HERE 0
# define FD_INDEX(x), x - 1

class Listener
{
private:
	struct pollfd _listener;
	std::vector<struct pollfd> _derived_socks;
	std::vector<Server> _assoc_servers;
	std::map<int, ARequest *> _requests;

	static const std::string request_types[];
	static void (Listener::* const creators [])(std::string const &);
public:
	Listener(std::string & where_to_listen);
	void addServer(Server & server);
	void addSocket(int fd);
	int getSockets(struct pollfd ** sockets) const;
	size_t getNumberofSockets(void) const;
	int getListenFd(void) const;

	void closeFds(void);
	int is_fd_here(int fd) const;
	void deleteFd(int fd);

//	TODO: update
	int updateRequest(int index, std::string buffer);
	void printRequest(int index);
	ARequest *createRequest(std::string & buffer);
	ARequest *createGet(std::string & init);

//	OCCF
	Listener(); //TODO: revisar OCCF
	Listener(const Listener &other);
	Listener &operator=(const Listener &other);
	~Listener();
};

#endif
