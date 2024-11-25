#include "ARequest.hpp"

#include <iostream>

int ARequest::appendRequest(std::string & append)
{
	static std::string raw;

	raw += append;

	for (size_t ind = raw.find(CRLF); ind != std::string::npos; ind = raw.find(CRLF))
	{
		if (this->_status == HEADERS)
			procHeader(raw, ind);
		// else if (this->_status == BODY)
		// {
		// 	this->_body = raw; //TODO: revisar fuerte, esto depende de headers y demas
		// 	this->_status = END;
		// 	return (this->_status);
		// }
		raw.erase(0, ind + 2);
	}
	return (this->_status);
}

void ARequest::procHeader(std::string & raw, size_t index)
{
	std::string header = raw.substr(0, index);
	size_t sep = header.find(":"); //TODO: errors y separador a lo peor es ": "
	std::string key = header.substr(0, sep);
	std::string value = header.substr(sep + 1);

	for (size_t i = 0; i < key.size(); i++)
		key[i] = tolower(key[i]);

	_headers[key] = value; //TODO: check for dups -> error code?
	if (raw[index + 2] == '\r' &&  raw[index + 3] == '\n') //TODO: podria ser mas limpio y podríamos quedarnos a medias...
	{
		_status = END; //TODO: si no hay body
		raw.erase(2);
	}
}

ARequest::ARequest(std::string const & uri) : _uri(uri), _status(HEADERS)
{}

//	GETTERs TODO: maybe its only for debug

std::string const&ARequest::getInitial(void)
{
	return (_uri);
}

std::string ARequest::getHeaders(void)
{
	std::string all_headers;

	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
	{
		all_headers += (it->first + ":" + it->second) + CRLF;
	}
	return (all_headers);
}

std::string &ARequest::getBody(void)
{
	return (_body);
}

std::ostream & operator<<(std::ostream & out, ARequest & req)
{
	out << req.getInitial() << req.getHeaders() << req.getBody();
	return (out);
}

//	OCCF

ARequest::ARequest(void)
{
#ifdef DEBUG
	std::cout << GREEN "ARequest default constructor called" NC << std::endl;
#endif
}

ARequest::ARequest(const ARequest &other) : _uri(other._uri), _headers(other._headers), _body(other._body), _status(other._status)
{
#ifdef DEBUG
	std::cout << YELLOW "ARequest copy constructor called" NC << std::endl;
#endif
	*this = other;
}

ARequest &ARequest::operator=(const ARequest &other)
{
#ifdef DEBUG
	std::cout << YELLOW "ARequest copy assignment operator called" NC << std::endl;
#endif
	(void)other;
	return (*this);
}

ARequest::~ARequest()
{
#ifdef DEBUG
	std::cout << RED "ARequest destrucutor called" NC << std::endl;
#endif
}
