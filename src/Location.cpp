#include "Location.hpp"
#include "delimiter.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>

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
	// TODO: ProcRule con cada instruccion separada en [key, value]
	std::vector<std::string> tmp;
	std::string buffer, key, value;
	std::size_t it;

	if (config[0] != STX)
    		buffer = config;
	else
    		buffer = config.substr(1, config.size() - 2);

	while (true) {
		it = buffer.find(US);
		if (it == std::string::npos) {
			tmp.push_back(buffer);
			break;
		}
		tmp.push_back(buffer.substr(0, it));
		buffer = buffer.substr(it + 1);
	}

	for (it = 0; it != tmp.size(); it++) {
		key = tmp[it].substr(0, tmp[it].find('='));
		value = tmp[it].substr(tmp[it].find('=') + 1, tmp[it].size());
		if (value.size() > 1 && *value.rbegin() == '/')
			value = value.substr(0, (value.size() - 1));
		std::cerr << __FILE__ << ": " << __LINE__ << " | [key: " << key << ", value: " << value << "]" << std::endl;
		procRule(key, value);
	}

	if (this->_root == "/")
		this->_deepness = 0;
	else
		this->_deepness = std::count(this->_root.begin(), this->_root.end(), '/');
	std::cerr << __FILE__ << ": " << __LINE__ << " | _deepness: " << this->_deepness << std::endl;
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
