#include "GETRequest.hpp"
#include "Server.hpp"
#include "Location.hpp"

#include <iostream>
#include <cstdlib>
#include <vector>

GETRequest::GETRequest(std::string const &uri) : ARequest(uri)
{
#ifdef DEBUG
	std::cout << GREEN "GETRequest constructor for uri " + uri + " called" NC << std::endl;
#endif
}

std::string GETRequest::generateResponse(std::vector<Server> & servers)
{
	std::string body = "<!DOCTYPE html>\n<html>\n<head>\n<title>Page Title</title>\n</head>\n<body>\n\n<h1>This is a Heading</h1>\n<p>This is a paragraph.</p>\n\n</body>\n</html>";
	std::string headers = "Content-Type: text/html\r\nContent-Length: 143\r\n\r\n";

	Location selected_loc = getSelectedLocation(servers);
	return selected_loc.responseGET(_uri, _query);

	return ("HTTP/1.1 200 OK\r\n" + headers + body);
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
