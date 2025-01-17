#ifndef AREQUEST_HPP
# define AREQUEST_HPP
# define CRLF "\r\n" //TODO: he leido que algunos clientes solo hacen LF

# include "colors.hpp"
# include "Server.hpp"

# include <string>
# include <map>
# include <iostream>
# include <vector>
# include <cstdio>

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
	std::string _query;
	std::string _path_info;
	std::map<std::string, std::string> _headers;
	std::string _body;
	std::vector<Server> _my_servers;
	int _status;
public:
	int appendRequest(std::string & append);
	void procHeader(std::string & raw, size_t index);

	ARequest(std::string const & uri, std::vector<Server> & servers);

	virtual std::string generateResponse(std::vector<Server> & servers) = 0;
	std::string const getHeaderValue(std::string const & key);

	Location const& getSelectedLocation(std::vector<Server> & servers);

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
