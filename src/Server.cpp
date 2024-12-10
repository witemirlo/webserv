#include "Server.hpp"
#include "delimiter.hpp"
#include "Location.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>

const std::string Server::rules[] = 
	{"listen", "server_name", "root", "index", "autoindex", "error_pages", "cgi_extension", ""};
void (Server::* const Server::setters [])(std::string const &) = 
	{&Server::setListen, &Server::setServerName, &Server::setRoot, &Server::setIndex, &Server::setAutoIndex, &Server::setErrorPages, &Server::setCGIExtension};

static bool has_delimiter(std::string const & str);

static std::map<int, std::string> default_error_pages(void)
{
	std::map<int, std::string> err_pags;

	err_pags[404] = DEF_404;

	return (err_pags);
}

/**
 * @param config a std::map of strings where the first indicates a directive and the second, the specification of that directive
 */
Server::Server(std::map<std::string, std::string> & config)
	: _listen(DEF_HOST ":" DEF_PORT),
	_server_name(std::vector<std::string> ()),
	_root (DEF_ROOT),
	_index(std::vector<std::string> ()),
	_autoindex(false),
	_error_pages(default_error_pages()),
	_cgi_extension("*")
{
	std::vector<std::string> loc_to_process;

	for (std::map<std::string, std::string>::iterator it = config.begin(); it != config.end(); it++)
	{
		if (!it->first.compare(0, 9, "location/"))
		{
			loc_to_process.push_back(it->first);
			continue ;
		}
		procRule(it->first, it->second);
	}
	setErrorPages("");
	for (std::vector<std::string>::iterator it = loc_to_process.begin(); it != loc_to_process.end(); it++)
	{
		//TODO: check for duplicated locations
		_locations[it->substr(8)] = Location(*this, config[*it], it->substr(8)); //TODO: a lo peor no se asigna bien
	}
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

/**
 * Checks if the name is present on this Server server_name
 */
bool Server::isNamed(std::string & name)
{
	for (std::vector<std::string>::iterator it = _server_name.begin(); it != _server_name.end(); it++)
	{
		if (!name.compare(*it))
			return true;
	}
	return false;
}

Location const& Server::getLocation(std::string const& uri) const
{
	std::map<std::string, Location>::const_iterator it;
	std::string                                     route, tmp;

	route = uri;
	
	if (*uri.begin() != '/')
		return this->_locations.at("/");
	
	while (route.size() > 1) {
		for (it = this->_locations.begin(); it != this->_locations.end(); it++) {
			tmp = it->first.substr(0, (it->first.find_last_of('/') + 1));
			if (tmp == route)
				return this->_locations.at(tmp);
		}
		route = route.substr(0, route.size() - 2);
		route = route.substr(0, route.find_last_of('/') + 1);
	}

	return this->_locations.at("/");
}

void Server::setIndex(std::string const &index)
{
	_index = setVector(index);
}

void Server::setErrorPages(std::string const &errors)
{
	if (_err_tmp.size() == 0)
	{
		_err_tmp = errors;
		return;
	}

	std::vector<std::string> error_pages = setVector(_err_tmp);
	for (size_t ind = 0; ind < error_pages.size(); ind++)
	{
		size_t eq = error_pages[ind].find("=");

		int num = atoi(error_pages[ind].substr(0, eq).c_str()); //TODO: lista de errores permitidos ERROR DE SYNTAX
		std::string	pages = error_pages[ind].substr(eq + 1); //TODO: check pages before

		_error_pages[num] = _root + pages;
	}
}

/**
 * Creates a vector of strings using str
 * 
 * @param str a std::string with the elements separed using US (ASCII number 2)
 */
std::vector<std::string> Server::setVector(std::string const & str)
{
	std::vector<std::string> result;
	
	if (!has_delimiter(str))
	{
		result.push_back(str);
		return result;
	}

	int ind = 0;
	for (size_t i = str.find(US); i != std::string::npos; i = str.find(US, ind + 1))
	{
		result.push_back(str.substr(ind, i - ind));
		ind = i;
	}
	result.push_back(str.substr(ind, str.size() - ind));

	return (result);
}

void Server::setCGIExtension(std::string const &cgi_extension)
{
	if (has_delimiter(cgi_extension))
	{
		std::cerr << RED "Error: " NC "\"cgi_extension\" directive cannot have delimiters \", [ ]\"" << std::endl;
		exit (EXIT_FAILURE);
	}
	for (size_t i = 0; i < cgi_extension.size(); i++)
	{
		if (!isalnum(cgi_extension[i]))
		{
			std::cerr << RED "Error: " NC "\"" + cgi_extension + "\" is not a valid extension. Only alphanumeric characters accepted" << std::endl;
			exit (EXIT_FAILURE);
		}
	}
	_cgi_extension = cgi_extension;
}

void Server::setRoot(std::string const &root)
{
	if (has_delimiter(root))
	{
		std::cerr << RED "Error: " NC "\"sever_name\" directive cannot have delimiters \", [ ]\"" << std::endl;
		exit (EXIT_FAILURE);		
	}
	_root = root;
}

void Server::setServerName(std::string const &server_name)
{
	_server_name = setVector(server_name);
}

void Server::setListen(std::string const &listen)
{
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

void Server::setAutoIndex(std::string const &autoindex)
{
	std::string format = autoindex.substr();

	for (size_t i = 0; i < autoindex.size(); i++)
		format[i] = tolower(autoindex[i]);

	if (!format.compare("true"))
		_autoindex = true;
	else if (!format.compare("false"))
		_autoindex = false;
	else
	{
		std::cerr << RED "Error: " NC "\"autoindex\" directive must be only \"true\" or \"false\"";
		exit(EXIT_FAILURE);
	}
}

//	GETTERs

std::vector<std::string> const& Server::getServerName(void) const
{
	return _server_name;
}

std::string const& Server::getListen(void) const
{
	return _listen;
}

std::string const& Server::getRoot(void) const
{
	return _root;
}

std::vector<std::string> const& Server::getIndex(void) const
{
	return _index;
}

bool Server::getAutoIndex(void) const
{
	return _autoindex;
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
#ifdef DEBUF
	std::cout << GREEN "Server default constructor called" NC << std::endl;
#endif
}

Server::Server(const Server &other) : 
	_locations(other._locations),
	_listen(other._listen),
	_server_name(other._server_name),
	_root(other._root),
	_index(other._index),
	_autoindex(other._autoindex),
	_error_pages(other._error_pages),
	_cgi_extension(other._cgi_extension)
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
	if (this == &other)
		return (*this);
		
	_listen = other._listen;
	_server_name = other._server_name;
	_root = other._root;
	_index = other._index;
	_autoindex = other._autoindex;
	_error_pages = other._error_pages;
	_cgi_extension = other._cgi_extension;
	_locations = other._locations;

	return (*this);
}

Server::~Server()
{
#ifdef DEBUG
	std::cout << RED "Server destrucutor called" NC << std::endl;
#endif
}
