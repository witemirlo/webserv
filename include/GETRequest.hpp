#ifndef GETREQUEST_HPP
# define GETREQUEST_HPP

# include "ARequest.hpp"

class GETRequest : public ARequest
{
public:
	GETRequest(std::string const &uri, std::vector<Server> & servers);
	std::string generateResponse(std::vector<Server> & servers);

//	OCCF
	GETRequest();
	GETRequest(const GETRequest &other);
	GETRequest &operator=(const GETRequest &other);
	~GETRequest();
};

#endif
