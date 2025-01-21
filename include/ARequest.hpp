#ifndef AREQUEST_HPP
# define AREQUEST_HPP
# define CRLF "\r\n"
# define SET_STATUS(old_s, new_s) ((old_s & (~0x3)) | new_s)
# define SET_ERROR(old_s, new_s) ((old_s & 0x3) | (new_s << 2))
# define GET_ERROR(status) (status >> 2)

# include "colors.hpp"
# include "Server.hpp"

# include <string>
# include <map>
# include <iostream>
# include <vector>
# include <cstdio>

enum request_status {
	INIT, 		//0b00
	HEADERS,	//0b01
	BODY,		//0b10
	END,		//0b11
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
	size_t _max_size;
	int _status;
public:
	int appendRequest(std::string & append);
	void procHeader(std::string & raw, size_t index);
	bool  is_valid_header(std::string const& header_key) const;

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
