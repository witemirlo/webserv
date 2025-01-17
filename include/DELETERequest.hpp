#ifndef _DELETEREQUEST_HPP
# define _DELETEREQUEST_HPP

# include "ARequest.hpp"

class DELETERequest : public ARequest {
public:
	DELETERequest(std::string const &uri, std::vector<Server> & servers);
	std::string generateResponse(std::vector<Server> & servers);

	DELETERequest(void);
	DELETERequest(DELETERequest const& other);
	~DELETERequest(void);
	DELETERequest& operator=(DELETERequest const& other);
};

#endif // !_DELETEREQUEST_HPP

