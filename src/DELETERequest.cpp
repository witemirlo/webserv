#include "DELETERequest.hpp"
#include "ARequest.hpp"
#include "Location.hpp"
#include "Server.hpp"

#include <iostream>

DELETERequest::DELETERequest(void) : ARequest()
{
	// std::cout << GREEN "DELETERequest default constructor called" NC << std::endl;
}

DELETERequest::DELETERequest(DELETERequest const& other) : ARequest(other)
{
	// std::cout << GREEN "DELETERequest copy constructor called" NC << std::endl;
	*this = other;
}

DELETERequest::~DELETERequest(void)
{
	// std::cout << RED "DELETERequest destructor called" NC << std::endl;
}

DELETERequest& DELETERequest::operator=(DELETERequest const& other)
{
	(void)other;
	return *this;
}

DELETERequest::DELETERequest(std::string const &uri, std::vector<Server> & servers) : ARequest(uri, servers)
{

}

std::string DELETERequest::generateResponse(std::vector<Server> & servers)
{
	if (_status != END)
		return getSelectedLocation(servers).responseGET(_status);
	return getSelectedLocation(servers).responseDELETE(_uri, _query);
}
