#include "GETRequest.hpp"
#include "Server.hpp"
#include "Location.hpp"
#include "HTTP_status_code.hpp"

#include <iostream>
#include <cstdlib>
#include <vector>

GETRequest::GETRequest(std::string const &uri, std::vector<Server> & servers) : ARequest(uri, servers)  {}

std::string GETRequest::generateResponse(std::vector<Server> & servers)
{
	Location selected_loc = getSelectedLocation(servers);
	if (selected_loc.getRedirections().find(_uri) != selected_loc.getRedirections().end())
		return selected_loc.responseGET(MOVED_PERMANENTLY, selected_loc.getRedirections().at(_uri));

	if (_status != END)
		return selected_loc.responseGET(GET_ERROR(_status));
	return selected_loc.responseGET(_uri, _query);

}

//	OCCF

GETRequest::GETRequest(void) {}

GETRequest::GETRequest(const GETRequest &other) : ARequest(other)
{
	*this = other;
}

GETRequest &GETRequest::operator=(const GETRequest &other)
{
	ARequest::operator=(other);
	return (*this);
}

GETRequest::~GETRequest() {}
