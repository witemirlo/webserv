#ifndef GETREQUEST_HPP
# define GETREQUEST_HPP

# include "ARequest.hpp"

class GETRequest : public ARequest
{
public:
	GETRequest(std::string const &uri);
	std::string generateResponse(void);

//	OCCF
	GETRequest();
	GETRequest(const GETRequest &other);
	GETRequest &operator=(const GETRequest &other);
	~GETRequest();
};

#endif
