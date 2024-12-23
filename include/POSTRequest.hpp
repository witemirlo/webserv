#ifndef POSTREQUEST_HPP
# define POSTREQUEST_HPP

# include "ARequest.hpp"

class POSTRequest : public ARequest
{
public:
	POSTRequest(std::string const &uri);
	std::string generateResponse(std::vector<Server> & servers);

//	OCCF
	POSTRequest();
	POSTRequest(const POSTRequest &other);
	POSTRequest &operator=(const POSTRequest &other);
	~POSTRequest();
};

#endif
