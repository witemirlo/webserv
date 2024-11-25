#include "Server.hpp"
#include "delimiter.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>

const std::string Server::rules[] = {"listen", "server_name", ""};
void (Server::* const Server::setters [])(std::string const &) = {&Server::setListen, &Server::setServerName};

static bool has_delimiter(std::string const & str);

/**
 * @param config a std::map of strings where the first indicates a directive and the second, the specification of that directive
 */
Server::Server(std::map<std::string, std::string> & config)
{
	for (std::map<std::string, std::string>::iterator it = config.begin(); it != config.end(); it++)
		procRule(it->first, it->second);
	//TODO: reglas fundamentales ej. listen
}

/**
 * Iters all the rules of the server to call the right setter
 * 
 * @param what the rule
 * @param to_set the specification of that rule
 */
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
	std::cerr << RED "Error: " NC "key \"" + what + "\" is invalid" << std::endl;
	exit(EXIT_FAILURE);
}

void Server::setServerName(std::string const &server_name)
{
	if (has_delimiter(server_name))
	{
		std::cerr << RED "Error: " NC "\"sever_name\" directive cannot have delimiters \", [ ]\"" << std::endl;
		exit (EXIT_FAILURE);
	}
	_server_name = server_name; //TODO: Y SI HAY VARIOS
}

void Server::setListen(std::string const &listen)
{
	// TODO: LOCALHOST:LOCALHOST:ETC
	if (has_delimiter(listen)) {
		std::cerr << RED "Error: " NC "\"listen\" directive cannot have delimiters \", [ ]\"" << std::endl;
		exit (EXIT_FAILURE);
	}

	if (listen.find(":") != std::string::npos) {
		if (listen.find(":") != listen.rfind(":")) {
			std::cerr << RED "Error: " NC "\"listen\" directive can only contain one \":\" character" << std::endl;
			exit(EXIT_FAILURE);
		}
		_listen = listen;
	}
	else {
		for (int i = 0; listen[i]; i++) {
			if (!isdigit(listen[i])) {
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

// OTHERS

static bool has_delimiter(std::string const & str)
{
	if (str.find(STX) != std::string::npos)
		return (true);	
	if (str.find(ETX) != std::string::npos)
		return (true);
	if (str.find(US) != std::string::npos)
		return (true);
	return (false);
}

//	OCCF

Server::Server(void)
{
	std::cout << GREEN "Server default constructor called" NC << std::endl;
}

Server::Server(const Server &other) : _listen(other._listen), _server_name(other._server_name)
{
#ifdef DEBUG
	std::cout << YELLOW "Server copy constructor called" NC << std::endl;
#endif
	*this = other;
}

Server &Server::operator=(const Server &other)
{
#ifdef DEBUG
	std::cout << YELLOW "Server copy assignment operator called" NC << std::endl;
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
