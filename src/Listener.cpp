#include "Listener.hpp"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>

static int get_listener(std::string & host, std::string & port);

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
}


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
		std::cerr << RED "getaddrinfo error: " + std::string(gai_strerror(err)) + NC << std::endl;
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
		std::cerr << RED "Failed to bind to host " + host + " at port " + port + NC << std::endl;
		perror("wevos: ");
		exit(EXIT_FAILURE);
	} //TODO: a lo mejor tendria que ser un return -1 y luego un throw... QUITAR PERROR

	if (listen(sfd, 10) == -1)
	{
		std::cerr << RED "Failed to bind to host listen" << std::endl;
		exit(EXIT_FAILURE);
	}

	return sfd;
}

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

//	OCCF

Listener::Listener(void)
{
	std::cout << GREEN "Listener default constructor called" NC << std::endl;
}

Listener::Listener(const Listener &other) : _listener(other._listener), _derived_socks(other._derived_socks), _assoc_servers(other._assoc_servers)
{
#ifdef DEBUG
	std::cout << YELOW "Listener copy constructor called" NC << std::endl;
#endif
	*this = other;
}

Listener &Listener::operator=(const Listener &other)
{
#ifdef DEBUG
	std::cout << YELOW "Listener copy assignment operator called" NC << std::endl;
#endif
	(void)other;
	return (*this);
}

Listener::~Listener()
{
#ifdef DEBUG
	std::cout << RED "Listener destrucutor called" NC << std::endl;
#endif
	close(_listener.fd);
	for (size_t i = 0; i < _derived_socks.size(); i++)
		close(_derived_socks[i].fd);
}
