#include "BADRequest.hpp"
#include "ARequest.hpp"
#include "Location.hpp"

#include <iostream>

BADRequest::BADRequest(void) : _code(500) {}
BADRequest::BADRequest(unsigned int code) : _code(code) {}
BADRequest::BADRequest(BADRequest const& other) { *this = other; }
BADRequest::~BADRequest(void) {}
BADRequest& BADRequest::operator=(BADRequest const& other) {
	this->_code = other._code;
	return *this;
}

std::string BADRequest::generateResponse(std::vector<Server> & servers)
{
	return getSelectedLocation(servers).responseGET(_code);
}
