#include "ARequest.hpp"
#include "get_config_data.hpp"

#include <iostream>
#include <cstdlib>

/**
 * Parses a string to update the request, changing the status to reflect were the message was left
 * 
 * @return the actual status of the request
 */
int ARequest::appendRequest(std::string & append)
{
	static std::string raw;

	raw.append(append);

	for (size_t ind = raw.find(CRLF); ind != std::string::npos; ind = raw.find(CRLF))
	{
		if (this->_status >= BODY)
			break ;
		if (this->_status == HEADERS)
			procHeader(raw, ind);
		raw.erase(0, ind + 2);
	}
	if (this->_status == BODY)
	{
		if ((size_t)std::atoll(_headers["content-length"].c_str()) <= raw.size())
		{
			_body = raw.substr();
			raw.erase();
			_status = END;
		}
	}
	return (this->_status);
}

/**
 * Proccess a string with a header declaration, splitting the key and value
 * 
 * @param index the index where it founded the last CRLF, so it knows if there are no more headers
 */
void ARequest::procHeader(std::string & raw, size_t index)
{
	if (index == 0)
	{
		try {
			_headers.at("content-length"); //TODO: transfer encoding
			if ((size_t)std::atoll(_headers["content-length"].c_str()) <= 0)
				throw std::exception();
			this->_status = BODY;
			if ((size_t)std::atoll(_headers["content-length"].c_str()) <= 0)
			return;
		} catch(const std::exception& e) {
			raw.erase();
			this->_status = END;
			return ;
		}
	}
	std::string header = raw.substr(0, index);
	size_t sep = header.find(":"); //TODO: errors
	std::string key = header.substr(0, sep);
	std::string value = trim(header.substr(sep + 1));

	for (size_t i = 0; i < key.size(); i++)
		key[i] = tolower(key[i]);

	_headers[key] = value; //TODO: check for dups -> error code?
}

/*
	Returns the Location object related to the specified URI of the Server referred by the 'host' header
*/
Location const& ARequest::getSelectedLocation(std::vector<Server> & servers)
{
	size_t selected = 0;
	std::string name = getHeaderValue(std::string("host"));

	if (name.size() != 0)
	{
		for (size_t ind = 0; ind < servers.size(); ind++)
		{
			if (servers[ind].isNamed(name))
			{
				selected = ind;
				break ;
			}
		}
	}

	return servers[selected].getLocation(_uri);
}

std::string const ARequest::getHeaderValue(std::string const & key)
{
	try {
		return (_headers.at(key));
	} catch (std::exception & e) {
		return ("");
	}
}

ARequest::ARequest(std::string const & uri, std::vector<Server> & servers) : _status(HEADERS)
{
	_my_servers = servers;

	size_t ind = uri.find('?');

	if (ind == std::string::npos)
		_uri = uri;
	else
	{
		_uri = uri.substr(0, ind);
		_query = uri.substr(ind + 1);
	}
}

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

ARequest::ARequest(const ARequest &other) : _uri(other._uri), _headers(other._headers), _body(other._body), _my_servers(other._my_servers), _status(other._status)
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
