#include "GETRequest.hpp"
#include "Server.hpp"
#include "Location.hpp"

#include <iostream>
#include <cstdlib>
#include <vector>

GETRequest::GETRequest(std::string const &uri, std::vector<Server> & servers) : ARequest(uri, servers)
{
#ifdef DEBUG
	std::cout << GREEN "GETRequest constructor for uri " + uri + " called" NC << std::endl;
#endif
}

std::string GETRequest::generateResponse(std::vector<Server> & servers)
{
	Location selected_loc = getSelectedLocation(servers);
	if (_status != END)
		return selected_loc.responseGET(_status);
	return selected_loc.responseGET(_uri, _query);

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
