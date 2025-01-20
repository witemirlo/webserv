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
	static ARequest* (Listener::* const creators [])(std::string const &, std::vector<Server> &);
public:
	Listener(std::string const& where_to_listen);
	void addServer(Server server);

//	fd and socket management

	void addSocket(int fd);
	int getSockets(struct pollfd ** sockets) const;
	size_t getNumberofSockets(void) const;
	int getListenFd(void) const;
	void closeFds(void);
	int is_fd_here(int fd) const;
	void deleteFd(int fd);
	void setFdToWrite(int fd);
	void setFdToRead(int fd);

//	Requests and reponses

	std::string respondTo(int fd);
	void printRequest(int index);
	int updateRequest(int index, std::string buffer);
	
	ARequest *createRequest(std::string & buffer, std::vector<Server> & servers);
	ARequest *createGet(std::string const & init, std::vector<Server> & servers);
	ARequest *createPost(std::string const & init, std::vector<Server> & servers);
	ARequest *createDelete(std::string const & init, std::vector<Server> & servers);

//	OCCF
	Listener();
	Listener(const Listener &other);
	Listener &operator=(const Listener &other);
	~Listener();
};

#endif
