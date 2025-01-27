#include "ARequest.hpp"
#include "get_config_data.hpp"
#include "HTTP_status_code.hpp"
#include "Location.hpp"

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
	static size_t body_size = 0;

	raw.append(append);

	for (size_t ind = raw.find(CRLF); ind != std::string::npos; ind = raw.find(CRLF))
	{
		if ((this->_status & END) >= BODY)
			break ;
		if ((this->_status & END) == HEADERS)
			procHeader(raw, ind);
		raw.erase(0, ind + 2);

		if (!_max_size)
		{
			try {
				_headers.at("host");
				_max_size = getSelectedLocation(_my_servers).getBodySize();
			}
			catch(const std::exception& e) {}
		}
	}
	if ((this->_status & END) == BODY)
	{
		if (GET_ERROR(_status) == CONTENT_TOO_LARGE)
		{
			body_size += raw.size();
			raw.erase();
		}
		else
			body_size = raw.size();
		if ((size_t)std::atoll(_headers["content-length"].c_str()) <= body_size)
		{
			_body = raw.substr();
			raw.erase();
			body_size = 0;
			_status = SET_STATUS(_status, END);
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
			_headers.at("content-length");
			if ((size_t)std::atoll(_headers["content-length"].c_str()) <= 0)
				throw std::exception();
			this->_status = SET_STATUS(this->_status, BODY);
			if ((size_t)std::atoll(_headers["content-length"].c_str()) > _max_size)
				this->_status = SET_ERROR(this->_status, CONTENT_TOO_LARGE);
			return;
		} catch(const std::exception& e) {
			raw.erase();
			this->_status = SET_STATUS(this->_status, END);
			return ;
		}
	}

	std::string header = raw.substr(0, index);
	if (header.length() < 3) {
		this->_status = SET_ERROR(this->_status, BAD_REQUEST);
		return ;
	}

	size_t sep = header.find(":");
	if (sep == std::string::npos) {
		this->_status = SET_ERROR(this->_status, BAD_REQUEST);
		return ;
	}

	std::string key = header.substr(0, sep);
	std::string value = trim(header.substr(sep + 1));

	if (key.length() == 0 || value.length() == 0 || !is_valid_header(key)) {
		this->_status = SET_ERROR(this->_status, BAD_REQUEST);
		return ;
	}

	for (size_t i = 0; i < key.size(); i++)
		key[i] = tolower(key[i]);

	if (this->_headers.find(key) != this->_headers.end()) {
		this->_status = SET_ERROR(this->_status, BAD_REQUEST);
		return ;
	}

	_headers[key] = value;
}

bool  ARequest::is_valid_header(std::string const& header_key) const
{
	for (std::size_t i = 0; header_key[i]; i++) {
		if (isspace(header_key[i]))
			return false;
	}
	return true;
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

ARequest::ARequest(std::string const & uri, std::vector<Server> & servers) : _max_size(0), _status(HEADERS)
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

//	OCCF

ARequest::ARequest(void) {}

ARequest::ARequest(const ARequest &other) : _uri(other._uri), _headers(other._headers), _body(other._body), _my_servers(other._my_servers), _max_size(other._max_size), _status(other._status)
{
	*this = other;
}

ARequest &ARequest::operator=(const ARequest &other)
{
	(void)other;
	return (*this);
}

ARequest::~ARequest() {}
