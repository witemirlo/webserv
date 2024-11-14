#include "Listener.hpp"

#include <iostream>

Listener::Listener(int fd)
{
	_listener.fd = fd;
	_listener.fd = POLLIN;
}

void Listener::addServer(Server & server)
{
	_assoc_servers.push_back(server);
}

//	OCCF

Listener::Listener(void)
{
	std::cout << GREEN "Listener default constructor called" NC << std::endl;
}

Listener::Listener(const Listener &other)
{
	std::cout << YELOW "Listener copy constructor called" NC << std::endl;
	*this = other;
}

Listener &Listener::operator=(const Listener &other)
{
	std::cout << YELOW "Listener copy assignment operator called" NC << std::endl;
	(void)other;
	return (*this);
}

Listener::~Listener()
{
	std::cout << RED "Listener destrucutor called" NC << std::endl;
}
