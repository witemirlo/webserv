#include "POSTRequest.hpp"
#include "Server.hpp"
#include "Location.hpp"
#include "HTTP_status_code.hpp"

#include <iostream>
#include <cstdlib>
#include <vector>

POSTRequest::POSTRequest(std::string const &uri, std::vector<Server> & servers) : ARequest(uri, servers)
{
#ifdef DEBUG
	std::cout << GREEN "POSTRequest constructor for uri " + uri + " called" NC << std::endl;
#endif
}

std::string POSTRequest::generateResponse(std::vector<Server> & servers)
{
	Location selected_loc = getSelectedLocation(servers);
	if (GET_ERROR(_status) != 0)
		return selected_loc.responseGET(GET_ERROR(_status));

	if (selected_loc.getRedirections().find(_uri) != selected_loc.getRedirections().end())
		return selected_loc.responseGET(MOVED_PERMANENTLY, selected_loc.getRedirections().at(_uri));

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
