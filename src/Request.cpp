#include "Request.hpp"

#include <iostream>

enum request_status Request::appendRequest(std::string & append)
{
	static std::string raw;

	if (raw.size()) //TODO: que jacobo lo pase a ternaria
		raw += append;
	else
		raw = append;

	for (size_t ind = raw.find(CRLF); ind != std::string::npos; ind = raw.find(CRLF))
	{
		if (_status == INIT)
		{
			_initial_line = raw.substr(ind + 2);
			_status == HEADERS;
		}
		else if (_status == HEADERS)
			procHeader(raw, ind);
		else if (_status == BODY)
		{
			_body = raw; //TODO: revisar fuerte, esto depende de headers y demas
			_status = END;
		}

		raw.erase(0, ind + 2);
	}

	return (_status);
}

void Request::procHeader(std::string & raw, size_t index)
{
	std::string header = raw.substr(index);
	size_t sep = header.find(":"); //TODO: errors y separador a lo peor es ": "
	std::string key = header.substr(sep); //TODO: toupper o tulower
	std::string value = header.substr(sep + 1, std::string::npos);

	_headers[key] = value; //TODO: check for dups
	if (raw[index + 2] == '\13' &&  raw[index + 3] == '\10') //TODO: podria ser mas limpio
	{
		_status = BODY; //TODO: si no hay body
		raw.erase(2);
	}
}

//	GETTERs TODO: maybe its only for debug

std::string &Request::getInitial(void)
{
	return (_initial_line);
}

std::string &Request::getHeaders(void)
{
	std::string all_headers;

	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
	{
		all_headers += (it->first + ":" + it->second) + CRLF;
	}
	return (all_headers);
}

std::string &Request::getBody(void)
{
	return (_body);
}

std::ostream & operator<<(std::ostream & out, Request & req)
{
	out << req.getInitial() << req.getHeaders() << req.getBody();
	return (out);
}

//	OCCF

Request::Request(void) : _status(INIT)
{
#ifdef DEBUG
	std::cout << GREEN "Request default constructor called" NC << std::endl;
#endif
}

Request::Request(const Request &other) : _initial_line(other._initial_line), _headers(other._headers), _body(other._body)
{
#ifdef DEBUG
	std::cout << YELLOW "Request copy constructor called" NC << std::endl;
#endif
	*this = other;
}

Request &Request::operator=(const Request &other)
{
#ifdef DEBUG
	std::cout << YELLOW "Request copy assignment operator called" NC << std::endl;
#endif
	(void)other;
	return (*this);
}

Request::~Request()
{
#ifdef DEBUG
	std::cout << RED "Request destrucutor called" NC << std::endl;
#endif
}
