#include "Server.hpp"

#include <iostream>
#include <sstream>

const std::string Server::rules[] = {"listen", "server_name", ""};
void (Server::* const Server::setters [])(std::string const &) = {&Server::setListen, &Server::setServerName};

Server::Server(std::map<std::string, std::string> & config)
{
	for (std::map<std::string, std::string>::iterator it = config.begin(); it != config.end(); it++)
	{
		procRule(it->first, it->second);
	}
}

void Server::procRule(std::string const &what, std::string const &to_set)
{

	for (int i = 0; rules[i].size(); i++)
	{
		if (!what.compare(rules[i].c_str()))
		{
			(this->*setters[i])(to_set);
			return ;
		}
	}
	//control de errores
}

void Server::setServerName(std::string const &server_name)
{
	_server_name = server_name; //Y SI HAY VARIOS
}

void Server::setListen(std::string const &listen)
{
	if (listen.find(":") != std::string::npos) //CHECK CHECK
		_listen = listen;
	else
	{
		for (int i = 0; listen[i]; i++)
		{
			if (!isdigit(listen[i]))
			{
				_listen = listen + ":" + DEF_PORT;
				return ;
			}
		}
		_listen = std::string(DEF_HOST) + ":" + listen;
	}
}

//	GETTERs

std::string &Server::getServerName(void)
{
	return _server_name;
}

std::string &Server::getListen(void)
{
	return _listen;
}

//	OCCF

Server::Server(void)
{
	std::cout << GREEN "Server default constructor called" NC << std::endl;
}

Server::Server(const Server &other) : _listen(other._listen), _server_name(other._server_name)
{
#ifdef DEBUG
	std::cout << YELOW "Server copy constructor called" NC << std::endl;
#endif
	*this = other;
}

Server &Server::operator=(const Server &other)
{
#ifdef DEBUG
	std::cout << YELOW "Server copy assignment operator called" NC << std::endl;
#endif
	(void)other;
	return (*this);
}

Server::~Server()
{
#ifdef DEBUG
	std::cout << RED "Server destrucutor called" NC << std::endl;
#endif
}
