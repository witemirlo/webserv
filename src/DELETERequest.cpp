#include "DELETERequest.hpp"
#include "Location.hpp"
#include "Server.hpp"

#include <iostream>

DELETERequest::DELETERequest(void)
{
	// std::cout << GREEN "DELETERequest default constructor called" NC << std::endl;
}

DELETERequest::DELETERequest(DELETERequest const& other)
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

DELETERequest::DELETERequest(std::string const &uri) : ARequest(uri)
{

}

std::string DELETERequest::generateResponse(std::vector<Server> & servers)
{
	Location selected_loc = getSelectedLocation(servers);
	std::string tmp; // TODO: borrar

	tmp = selected_loc.responseDELETE(_uri, _query);
	std::cerr << __FILE__ << ": " << __LINE__ << ": DELETERequest::generateResponse:\n" << tmp;
	return tmp;
	// return getSelectedLocation(servers).responseDELETE(_uri, _query);
}
