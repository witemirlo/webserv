#include "Server.hpp"

#include <iostream>
#include <sstream>

const std::string Server::rules[] = {"listen", "server_name", ""};
void (Server::* const Server::setters [])(std::string &) = {&Server::setListen, &Server::setServerName};

Server::Server(std::string const &config)
{
	std::stringstream str(config);
	std::string rule;

	while (getline(str, rule))
        procRule(rule);
}

void Server::procRule(std::string &rule)
{
	size_t eq_index = rule.find(" = "); //CHECKEAR
	std::string what = rule.substr(0, eq_index);
	std::string to_set = rule.substr(eq_index + 3, std::string::npos);

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

void Server::setServerName(std::string &server_name)
{
	_server_name = server_name; //Y SI HAY VARIOS
}

void Server::setListen(std::string &listen)
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
