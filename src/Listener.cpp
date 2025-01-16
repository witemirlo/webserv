#include "Listener.hpp"
#include "GETRequest.hpp"
#include "POSTRequest.hpp"
#include "DELETERequest.hpp"
#include "BADRequest.hpp"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <sstream>

const std::string Listener::request_types[] = {"GET", "POST", "DELETE", ""};
ARequest* (Listener::* const Listener::creators [])(std::string const &) = {&Listener::createGet, &Listener::createPost, &Listener::createDelete};

static int get_listener(std::string & host, std::string & port);

/**
 * Updates the request of the socket fd with new information. If it doesnt exists, it creates it.
 * 
 * @return the status of the request
 */
int Listener::updateRequest(int fd, std::string buffer, int bytes_read)
{
	try {
		return (_requests.at(fd)->appendRequest(buffer, bytes_read));
	} catch (std::exception const & e) {
		_requests[fd] = createRequest(buffer);
		return (_requests.at(fd)->appendRequest(buffer, bytes_read));
	}
}


ARequest *Listener::createRequest(std::string & buffer)
{
	// NOTE: index del CRLF
	// std::stringstream tmp(buffer);
	// std::string method, uri;

	// tmp >> method;
	// tmp >> uri;

	// if (tmp.str() != CRLF) {
	// 	// TODO: error check
	// 	return NULL;
	// }
	//----------------------------------------------------------------------
	std::size_t ind = buffer.find(CRLF); //TODO: he leido que algunos empiezan con CRLF y tecnicamente podría pasar que no esté en el primer read
	if (ind == std::string::npos) {
		// TODO: control de errores
		return new BADRequest(400);
		return NULL;
	}

	std::string request_line = buffer.substr(0, ind + 2); // linea de la peticion (GET /)

	size_t sp = request_line.find(" "); // TODO: un unico espacio? o cualquier espacio?
	std::string method = request_line.substr(0, sp); // NOTE: primera palabra (GET, ...)
	request_line.erase(0, sp + 1);

	sp = request_line.find(" ");
	std::string uri = request_line.substr(0, sp); // NOTE: segunda palabra
	buffer.erase(0, ind + 2);
	if (uri.size() == 0) {
		// TODO: control de errores
		return new BADRequest(400);
		return NULL;
	}

	for (int i = 0; request_types[i].size(); i++)
	{
		// if (!method.compare(request_types[i])) // TODO: borrar
		if (method == request_types[i])
			return ((this->*creators[i])(uri));
	}
	return new BADRequest(400);
	return (NULL); //TODO: error management
}

ARequest *Listener::createGet(std::string const & init)
{
	return (new GETRequest(init));
}

ARequest *Listener::createPost(std::string const & init)
{
	return (new POSTRequest(init));
}

ARequest *Listener::createDelete(std::string const & init)
{
	return (new DELETERequest(init));
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
}
