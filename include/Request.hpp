#ifndef REQUEST_HPP
# define REQUEST_HPP
# define CRLF "\13\10" //TODO: he leido que algunos clientes solo hacen LF

# include "colors.hpp"

# include <string>
# include <map>
# include <iostream>

enum request_status {
	INIT,
	HEADERS,
	BODY,
	END,
	ERROR,
};

class Request
{
private:
	std::string _initial_line; //TODO: cambiar a method y url??
	std::map<std::string, std::string> _headers;
	std::string _body;
	enum request_status _status;
public:
	enum request_status appendRequest(std::string & append);
	void procHeader(std::string & raw, size_t index);

//	GETTERs TODO: maybe its only for debug
	std::string &getInitial(void);
	std::string &getHeaders(void);
	std::string &getBody(void);

//	OCCF
	Request();
	Request(const Request &other);
	Request &operator=(const Request &other);
	~Request();
};

std::ostream & operator<<(std::ostream & out, Request & req);

#endif
