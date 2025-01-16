#include "POSTRequest.hpp"
#include "Server.hpp"
#include "Location.hpp"

#include <iostream>
#include <cstdlib>
#include <vector>

POSTRequest::POSTRequest(std::string const &uri) : ARequest(uri)
{
#ifdef DEBUG
	std::cout << GREEN "POSTRequest constructor for uri " + uri + " called" NC << std::endl;
#endif
}

std::string POSTRequest::generateResponse(std::vector<Server> & servers)
{
	Location selected_loc = getSelectedLocation(servers);
	return selected_loc.responsePOST(_uri, _body, getHeaderValue("content-type"), getHeaderValue("content-length"));
}

//	OCCF

POSTRequest::POSTRequest(void)
{
#ifdef DEBUG
	std::cout << GREEN "POSTRequest default constructor called" NC << std::endl;
#endif
}

POSTRequest::POSTRequest(const POSTRequest &other) : ARequest(other)
{
#ifdef DEBUG
	std::cout << YELLOW "POSTRequest copy constructor called" NC << std::endl;
#endif
	*this = other;
}

POSTRequest &POSTRequest::operator=(const POSTRequest &other)
{
#ifdef DEBUG
	std::cout << YELLOW "POSTRequest copy assignment operator called" NC << std::endl;
#endif
	ARequest::operator=(other);
	return (*this);
}

POSTRequest::~POSTRequest()
{
#ifdef DEBUG
	std::cout << RED "POSTRequest destrucutor called" NC << std::endl;
#endif
}
