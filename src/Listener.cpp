#include "BADRequest.hpp"
#include "DELETERequest.hpp"
#include "GETRequest.hpp"
#include "HTTP_status_code.hpp"
#include "Listener.hpp"
#include "POSTRequest.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

const std::string Listener::request_types[] = {"GET", "POST", "DELETE", ""};
ARequest* (Listener::* const Listener::creators [])(std::string const &, std::vector<Server> &) = {&Listener::createGet, &Listener::createPost, &Listener::createDelete};

static int get_listener(std::string & host, std::string & port);

/**
 * Updates the request of the socket fd with new information. If it doesnt exists, it creates it.
 * 
 * @return the status of the request
 */
int Listener::updateRequest(int fd, std::string buffer)
{
	try {
		return (_requests.at(fd)->appendRequest(buffer));
	} catch (std::exception const & e) {
		_requests[fd] = createRequest(buffer,_assoc_servers);
		return (_requests.at(fd)->appendRequest(buffer));
	}
}


ARequest *Listener::createRequest(std::string & buffer, std::vector<Server> & servers)
{
	std::size_t ind = buffer.find(CRLF);
	if (ind == std::string::npos) {
		if (std::count(buffer.begin(), buffer.end(), ' ') == 0)
			return new BADRequest(NOT_IMPLEMENTED);

		if (std::count(buffer.begin(), buffer.end(), ' ') == 1)
			return new BADRequest(URI_TOO_LONG);

		return new BADRequest(BAD_REQUEST);
	}

	std::string request_line = buffer.substr(0, ind/*  + 2 */);
	if (std::count(request_line.begin(), request_line.end(), ' ') != 2)
		return new BADRequest(BAD_REQUEST);

	size_t sp = request_line.find(" ");
	std::cerr << __FILE__ << ":" << __LINE__ << ": resquest_line: " << request_line << std::endl; // TODO: limpiar el uri de la request y comprobar su cotenido (el HTTP/1.1)
	std::string method = request_line.substr(0, sp);
	request_line.erase(0, sp + 1);
	std::cerr << __FILE__ << ":" << __LINE__ << ": method: " << method << std::endl; // TODO: limpiar el uri de la request y comprobar su cotenido (el HTTP/1.1)
	std::cerr << __FILE__ << ":" << __LINE__ << ": resquest_line: " << request_line << std::endl; // TODO: limpiar el uri de la request y comprobar su cotenido (el HTTP/1.1)

	sp = request_line.find(" ");
	std::string uri = request_line.substr(0, sp); // NOTE: segunda palabra
	std::cerr << __FILE__ << ":" << __LINE__ << ": uri: " << uri << std::endl; // TODO: limpiar el uri de la request y comprobar su cotenido (el HTTP/1.1)
	request_line.erase(0, sp + 1);
	std::cerr << __FILE__ << ":" << __LINE__ << ": resquest_line: |" << request_line << "|" << std::endl; // TODO: limpiar el uri de la request y comprobar su cotenido (el HTTP/1.1)
	buffer.erase(0, ind + 2);

	if (uri.size() == 0)
		return new BADRequest(BAD_REQUEST);

	if (request_line != "HTTP/1.1") {
		if (!request_line.compare(0, 5, "HTTP/"))
			return new BADRequest(HTTP_VERSION_NOT_SUPPORTED);
		return new BADRequest(BAD_REQUEST);
	}

	for (int i = 0; request_types[i].size(); i++) {
		if (method == request_types[i])
			return ((this->*creators[i])(uri, servers));
	}

	return new BADRequest(NOT_IMPLEMENTED);
}

ARequest *Listener::createGet(std::string const & init, std::vector<Server> & servers)
{
	return (new GETRequest(init, servers));
}

ARequest *Listener::createPost(std::string const & init, std::vector<Server> & servers)
{
	return (new POSTRequest(init, servers));
}

ARequest *Listener::createDelete(std::string const & init, std::vector<Server> & servers)
{
	return (new DELETERequest(init, servers));
}

void Listener::printRequest(int index)
{
	std::cout << "My request: ";
	std::cout << (ARequest *)(_requests[index]) << std::endl;
}

/**
 *
 * @param where_to_listen an alreay formated "host:port" string.
 */
Listener::Listener(std::string const & where_to_listen)
{
	size_t ind = where_to_listen.find(':');
	std::string host = where_to_listen.substr(0, ind);
	std::string port = where_to_listen.substr(ind + 1, std::string::npos);

	int fd = get_listener(host, port);
	_listener.fd = fd;
	_listener.events = POLLIN;
}

void Listener::addServer(Server server)
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
	int size = getNumberofSockets();
	struct pollfd * scks = new struct pollfd [size];

	memset(scks, 0, sizeof(struct pollfd) * size);
	memcpy(scks, &_listener, sizeof(struct pollfd));
	std::copy(_derived_socks.begin(), _derived_socks.end(), scks + 1);
	size_t next = _derived_socks.size() + 1;
	for (std::map<int, struct pollfd>::const_iterator it = _cgi_sockets.begin(); it != _cgi_sockets.end(); it++)
	{
		memcpy(scks + next, &(it->second), sizeof(struct pollfd));
		next++;
	}
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
	for (std::map<int, struct pollfd>::const_iterator it = _cgi_sockets.begin(); it != _cgi_sockets.end(); it++)
	{
		if (fd == it->second.fd)
			return (_derived_socks.size() + it->first);
	}
	return (FD_NOT_HERE);
}

bool Listener::is_cgi_socket(int fd) const
{
	for (std::map<int, struct pollfd>::const_iterator it = _cgi_sockets.begin(); it != _cgi_sockets.end(); it++)
	{
		if (it->second.fd == fd)
			return true;
	}
	return false;
}

size_t Listener::getNumberofSockets(void) const
{
	return (_derived_socks.size() + _cgi_sockets.size() + 1);
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

void Listener::setFdToWrite(int fd)
{
	for (size_t i = 0; i < _derived_socks.size(); i++)
	{
		if (fd == _derived_socks[i].fd)
		{
			_derived_socks[i].events = POLLOUT;
			return ;
		}
	}
}

void Listener::setFdToRead(int fd)
{
	for (size_t i = 0; i < _derived_socks.size(); i++)
	{
		if (fd == _derived_socks[i].fd)
		{
			_derived_socks[i].events = POLLIN;
			return ;
		}
	}
}

/**
 * Creates a reponse to the request of fd and sets it to wait to read
 */
std::string Listener::respondTo(int fd)
{
	std::string response = _requests[fd]->generateResponse(_assoc_servers);

	delete _requests[fd];
	_requests.erase(fd);
	setFdToRead(fd);
	// std::cerr << __FILE__ << ":" << __LINE__ << " | " << "Listener::respondTo returns:\n" << response << std::endl;
	return (response);
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

	for (std::map<int, struct pollfd>::const_iterator it = _cgi_sockets.begin(); it != _cgi_sockets.end(); it++)
		close(it->second.fd);
}
