#ifndef AREQUEST_HPP
# define AREQUEST_HPP
# define CRLF "\r\n" //TODO: he leido que algunos clientes solo hacen LF

# include "colors.hpp"

# include <string>
# include <map>
# include <iostream>

enum request_status {
	INIT,
	HEADERS,
	BODY,
	END,
	ERROR
};

class ARequest
{
private:
	std::string _initial_line; //TODO: cambiar a method y url??
	std::map<std::string, std::string> _headers;
	std::string _body;
	int _status;
public:
	int appendRequest(std::string & append);
	void procHeader(std::string & raw, size_t index);

//	GETTERs TODO: maybe its only for debug
	std::string const&getInitial(void);
	std::string getHeaders(void);
	std::string &getBody(void);

//	OCCF
	ARequest();
	ARequest(const ARequest &other);
	ARequest &operator=(const ARequest &other);
	~ARequest();
};

std::ostream & operator<<(std::ostream & out, ARequest & req);

#endif
