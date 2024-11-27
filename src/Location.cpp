#include "Location.hpp"
#include "delimiter.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>

const std::string Location::prohibited_rules[] =  {"listen", "server_name", ""};

Location::Location(void)
	: Server()
{
#ifdef DEBUG
	std::cout << GREEN "Location default constructor called" NC << std::endl;
#endif
}

Location::Location(const Location &other)
	: Server(dynamic_cast<Server const&>(other))
{
#ifdef DEBUG
	std::cout << YELLOW "Location copy constructor called" NC << std::endl;
#endif
	*this = other;
}

Location::~Location()
{
#ifdef DEBUG
	std::cout << RED "Location destrucutor called" NC << std::endl;
#endif
}


Location::Location(Server const& o, std::string const & config)
	: Server(o)
{
#ifdef DEBUG
	std::cout << GREEN "Location constructor called" NC << std::endl;
#endif
	std::string buffer, line, key, value;
	std::size_t it;

	if (config[0] != STX)
    		buffer = config;
	else
    		buffer = config.substr(1, config.size() - 2);

	while (true) {
		it = buffer.find(US);
		if (it == std::string::npos)
			line = buffer;
		else
			line = buffer.substr(0, it);

		key = line.substr(0, line.find('='));
		value = line.substr(line.find('=') + 1, line.size());
		if (value.size() > 1 && *value.rbegin() == '/')
			value = value.substr(0, (value.size() - 1));
		/*
		if key en prohibidas
			std::cerr key solo se permite en server
		*/

		procRule(key, value);

		if (it == std::string::npos)
			break;
		else
			buffer = buffer.substr(it + 1);
	}

	if (this->_root == "/")// jeje xd TODO: es del pobre nombre de location
		this->_deepness = 0;
	else
		this->_deepness = std::count(this->_root.begin(), this->_root.end(), '/');
	std::cerr << this->_deepness << std::endl;
}

Location &Location::operator=(const Location &other)
{
#ifdef DEBUG
	std::cout << YELLOW "Location copy assignment operator called" NC << std::endl;
#endif
	if (this == &other)
		return (*this);

	this->_deepness = other._deepness;
	return (*this);
}

bool Location::operator>(const Location & other) const
{
	return (this->_deepness > other._deepness);
}

bool Location::operator<(const Location & other) const
{
	return (this->_deepness < other._deepness);
}

bool Location::operator>=(const Location & other) const
{
	return (this->_deepness >= other._deepness);
}

bool Location::operator<=(const Location & other) const
{
	return (this->_deepness <= other._deepness);
}

std::string Location::getPathTo(std::string const & uri) const
{
	// TODO: autoindex true y te pasan directorio
	// TODO: index seteado y te pasan directorio
	// TODO: 404
	return (this->_root + uri);
}
