#include "ARequest.hpp"
#include "get_config_data.hpp"

#include <iostream>
#include <cstdlib>

int ARequest::appendRequest(std::string & append)
{
	static std::string raw;

	raw += append;

	for (size_t ind = raw.find(CRLF); ind != std::string::npos; ind = raw.find(CRLF))
	{
		if (this->_status == HEADERS)
			procHeader(raw, ind);
		raw.erase(0, ind + 2);
	}
	if (_status == BODY)
	{
		if ((size_t)std::atoll(_headers["content-length"].c_str()) <= raw.size())
		{
			_body = raw.substr();
			_status = END;
		}
	}
	return (this->_status);
}

void ARequest::procHeader(std::string & raw, size_t index)
{
	if (index == 0)
	{
		try {
			_headers.at("content-length"); //TODO: transfer encoding
			this->_status = BODY;
			return;
		} catch(const std::exception& e) {
			this->_status = END;
			return ;
		}
	}
	std::string header = raw.substr(0, index);
	// std::cout << "CRLF index: " << index << " for header -> " << header << std::endl;
	size_t sep = header.find(":"); //TODO: errors
	std::string key = header.substr(0, sep);
	std::string value = trim(header.substr(sep + 1));

	for (size_t i = 0; i < key.size(); i++)
		key[i] = tolower(key[i]);

	_headers[key] = value; //TODO: check for dups -> error code?
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

std::ostream & operator<<(std::ostream & out, ARequest * req)
{
	out << req->getInitial() << req->getHeaders() << req->getBody();
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
