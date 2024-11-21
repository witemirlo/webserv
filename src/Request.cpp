#include "Request.hpp"

#include <iostream>

Request::Request(void)
{
#ifdef DEBUG
	std::cout << GREEN "Request default constructor called" NC << std::endl;
#endif
}

Request::Request(const Request &other) : _initial_line(other._initial_line), _headers(other._headers), _body(other._body)
{
#ifdef DEBUG
	std::cout << YELLOW "Request copy constructor called" NC << std::endl;
#endif
	*this = other;
}

Request &Request::operator=(const Request &other)
{
#ifdef DEBUG
	std::cout << YELLOW "Request copy assignment operator called" NC << std::endl;
#endif
	(void)other;
	return (*this);
}

Request::~Request()
{
#ifdef DEBUG
	std::cout << RED "Request destrucutor called" NC << std::endl;
#endif
}
