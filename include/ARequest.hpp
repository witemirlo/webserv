#ifndef AREQUEST_HPP
# define AREQUEST_HPP
# define CRLF "\r\n" //TODO: he leido que algunos clientes solo hacen LF

# include "colors.hpp"
# include "Server.hpp"

# include <string>
# include <map>
# include <iostream>
# include <vector>

enum request_status {
	INIT,
	HEADERS,
	BODY,
	END,
	ERROR
};

class ARequest
{
protected:
	std::string _uri;
	std::map<std::string, std::string> _headers;
	std::string _body;
	int _status;
public:
	int appendRequest(std::string & append);
	void procHeader(std::string & raw, size_t index);

	ARequest(std::string const & uri);

	virtual std::string generateResponse(std::vector<Server> & servers) = 0;
	std::string const getHeaderValue(std::string const & key);

//	GETTERs TODO: maybe its only for debug
	std::string const&getInitial(void);
	std::string getHeaders(void);
	std::string &getBody(void);

//	OCCF
	ARequest();
	ARequest(const ARequest &other);
	ARequest &operator=(const ARequest &other);
	virtual ~ARequest();
};

std::ostream & operator<<(std::ostream & out, ARequest * req);

#endif
