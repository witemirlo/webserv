#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "colors.hpp"

# include <string>
# include <map>

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
public:
	enum request_status append_request(std::string & append);

//	OCCF
	Request();
	Request(const Request &other);
	Request &operator=(const Request &other);
	~Request();
};

#endif
