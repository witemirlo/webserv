#include "Listener.hpp"
#include "GETRequest.hpp"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>

const std::string Listener::request_types[] = {"GET", ""};
ARequest* (Listener::* const Listener::creators [])(std::string const &) = {&Listener::createGet};

static int get_listener(std::string & host, std::string & port);

int Listener::updateRequest(int index, std::string buffer)
{
	try {
		return (_requests[index]->appendRequest(buffer));
	} catch (std::exception const & e) {
		_requests[index] = createRequest(buffer);
		return (_requests[index]->appendRequest(buffer));
	}
}

ARequest *Listener::createRequest(std::string & buffer)
{
	size_t ind = buffer.find(CRLF); //TODO: he leido que algunos empiezan con CRLF y tecnicamente podría pasar que no esté en el primer read
	std::string request_line = buffer.substr(0, ind + 2);
	size_t sp = request_line.find(" ");
	std::string method = request_line.substr(0, sp);
	request_line.erase(sp);
	sp = request_line.find(" ");
	std::string uri = request_line.substr(0, sp);
	buffer.erase(ind + 2);

	for (int i = 0; request_types[i].size(); i++)
	{
		if (!method.compare(request_types[i]))
			return ((this->*creators[i])(uri));
	}
	return (NULL); //TODO: error management
}

ARequest *Listener::createGet(std::string const & init)
{
	return (new GETRequest(init));
}

void Listener::printRequest(int index)
{
	std::cout << _requests[index];
}

/**
 *
 * @param where_to_listen an alreay formated "host:port" string.
 */
Listener::Listener(std::string & where_to_listen)
{
	size_t ind = where_to_listen.find(':');
	std::string host = where_to_listen.substr(0, ind);
	std::string port = where_to_listen.substr(ind + 1, std::string::npos);

	int fd = get_listener(host, port);
	_listener.fd = fd;
	_listener.events = POLLIN;
}

void Listener::addServer(Server & server)
{
	_assoc_servers.push_back(server);
}

void Listener::addSocket(int fd)
{
	struct pollfd skc;

	skc.fd = fd;
	skc.events = POLLIN;
	_derived_socks.push_back(skc);
	// _requests[fd] = Request ();
}

/**
 * Creates a socket, binds it to an specified address and marks it as a passive socket
 * 
 * @param host the host to bind to
 * @param port the port to bind to
 * @return the fd of the created socket	
 */
static int get_listener(std::string & host, std::string & port)
{
	struct addrinfo req;
	struct addrinfo *res;
	int err;

	memset(&req, 0, sizeof req);
	req.ai_family = AF_UNSPEC; 
	req.ai_socktype = SOCK_STREAM;
	req.ai_flags = AI_PASSIVE;

	if ((err = getaddrinfo(host.c_str(), port.c_str(), &req, &res)) != 0)
	{
		std::cerr << RED "Error: " NC "getaddrinfo [" + host + ":" + port + "] " + std::string(gai_strerror(err)) << std::endl;
		exit(1);
	}

	int sfd;
	struct addrinfo *test;
	for (test = res; test; test = test->ai_next)
	{
		sfd = socket(test->ai_family, test->ai_socktype, test->ai_protocol);
		if (sfd == -1)
			continue;
		if (bind(sfd, test->ai_addr, test->ai_addrlen) == -1)
		{
			close(sfd);
			continue;
		}
		break;
	}	
	freeaddrinfo(res);
	if (test == NULL) 
	{
		std::cerr << RED "Error: " NC "Failed to bind to host " + host + " at port " + port << std::endl;
		exit(EXIT_FAILURE);
	}

	if (listen(sfd, SOMAXCONN) == -1)
	{
		std::cerr << RED "Error: " NC "Failed to listen to fd: " << sfd << std::endl;
		exit(EXIT_FAILURE);
	}

	return sfd;
}

/**
 * Creates an array of struct pollfd of all the sockets contained in this object, including the listening socket
 * 
 * @param sockets the address to where to store the structs pollfd
 * @return the number of sockets
 */
int Listener::getSockets(struct pollfd ** sockets) const
{
	int size = _derived_socks.size() + 1;
	struct pollfd * scks = new struct pollfd [size];

	memset(scks, 0, size);
	memcpy(scks, &_listener, sizeof(struct pollfd));
	std::copy(_derived_socks.begin(), _derived_socks.end(), scks + 1);
	*sockets = scks;
	return (size);
}

/**
 * Searchs if a given file descriptor is withing this object
 * 
 * @param fd the fd to search
 * @return FD_IS_LISTENER if fd is the listener socket; FD_NOT_HERE if the fd is not here; a positive value if the fd is a socket derived from the listener
 */
int Listener::is_fd_here(int fd) const
{
	if (fd == _listener.fd)
		return FD_IS_LISTENER;
	for (size_t i = 0; i < _derived_socks.size(); i++)
	{
		if (fd == _derived_socks[i].fd)
			return (i + 1);
	}
	return (FD_NOT_HERE);
}

size_t Listener::getNumberofSockets(void) const
{
	return (_derived_socks.size() + 1);
}

int Listener::getListenFd(void) const
{
	return _listener.fd;
}

void Listener::deleteFd(int fd)
{
	//TODO: exception?
	for (size_t i = 0; i < _derived_socks.size(); i++)
	{
		if (fd == _derived_socks[i].fd)
		{
			close(fd); //TODO: esto falla?
			_derived_socks.erase(_derived_socks.begin() + i);
			_requests.erase(fd); //TODO: delete?
			return ;
		}
	}

}

//	OCCF

Listener::Listener(void)
{
	std::cout << GREEN "Listener default constructor called" NC << std::endl;
}

Listener::Listener(const Listener &other) : _listener(other._listener), _derived_socks(other._derived_socks), _assoc_servers(other._assoc_servers)
{
#ifdef DEBUG
	std::cout << YELLOW "Listener copy constructor called" NC << std::endl;
#endif
	*this = other;
}

Listener &Listener::operator=(const Listener &other)
{
#ifdef DEBUG
	std::cout << YELLOW "Listener copy assignment operator called" NC << std::endl;
#endif
	(void)other;
	return (*this);
}

Listener::~Listener()
{
#ifdef DEBUG
	std::cout << RED "Listener destrucutor called" NC << std::endl;
#endif
}

void Listener::closeFds(void)
{
	close(_listener.fd);
	for (size_t i = 0; i < _derived_socks.size(); i++)
		close(_derived_socks[i].fd);
}
