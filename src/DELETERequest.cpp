#include "ARequest.hpp"
#include "DELETERequest.hpp"
#include "HTTP_status_code.hpp"
#include "Location.hpp"
#include "Server.hpp"

#include <iostream>

DELETERequest::DELETERequest(void) : ARequest()
{
	// std::cout << GREEN "DELETERequest default constructor called" NC << std::endl;
}

DELETERequest::DELETERequest(DELETERequest const& other) : ARequest()
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
	Location tmp = getSelectedLocation(servers);

	if (_status != END)
		return tmp.responseGET(GET_ERROR( _status));
	
	if (tmp.getRedirections().find(_uri) != tmp.getRedirections().end())
		return tmp.responseGET(MOVED_PERMANENTLY, tmp.getRedirections().at(_uri));

	return tmp.responseDELETE(_uri, _query);
}
