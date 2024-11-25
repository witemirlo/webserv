#include "GETRequest.hpp"

#include <iostream>

GETRequest::GETRequest(std::string const &uri) : ARequest(uri)
{
}

//	OCCF

GETRequest::GETRequest(void)
{
#ifdef DEBUG
	std::cout << GREEN "GETRequest default constructor called" NC << std::endl;
#endif
}

GETRequest::GETRequest(const GETRequest &other) : ARequest(other)
{
#ifdef DEBUG
	std::cout << YELLOW "GETRequest copy constructor called" NC << std::endl;
#endif
	*this = other;
}

GETRequest &GETRequest::operator=(const GETRequest &other)
{
#ifdef DEBUG
	std::cout << YELLOW "GETRequest copy assignment operator called" NC << std::endl;
#endif
	ARequest::operator=(other);
	return (*this);
}

GETRequest::~GETRequest()
{
#ifdef DEBUG
	std::cout << RED "GETRequest destrucutor called" NC << std::endl;
#endif
}
