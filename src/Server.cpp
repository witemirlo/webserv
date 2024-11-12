#include "Server.hpp"

#include <iostream>
#include <sstream>

const std::string Server::rules[] = {"listen", "server_name", 0};

Server::Server(std::string &config)
{
	std::stringstream str(config);
	std::string rule;

	while (getline(str, rule))
        procRule(rule);
}

Server::Server(void)
{
	std::cout << GREEN "Server default constructor called" NC << std::endl;
}

Server::Server(const Server &other)
{
	std::cout << YELOW "Server copy constructor called" NC << std::endl;
	*this = other;
}

Server &Server::operator=(const Server &other)
{
	std::cout << YELOW "Server copy assignment operator called" NC << std::endl;
	return (*this);
}

Server::~Server()
{
	std::cout << RED "Server destrucutor called" NC << std::endl;
}
