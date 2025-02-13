#include "BADRequest.hpp"
#include "ARequest.hpp"
#include "Location.hpp"

#include <iostream>

BADRequest::BADRequest(void) : ARequest(), _code(500) {}
BADRequest::BADRequest(BADRequest const& other) : ARequest() { *this = other; }
BADRequest::~BADRequest(void) {}
BADRequest& BADRequest::operator=(BADRequest const& other) {
	this->_code = other._code;
	return *this;
}

BADRequest::BADRequest(unsigned int code)
	: ARequest()
	, _code(code)
{
	_status = HEADERS;
}

std::string BADRequest::generateResponse(std::vector<Server> & servers)
{
	return getSelectedLocation(servers).responseGET(_code);
}
