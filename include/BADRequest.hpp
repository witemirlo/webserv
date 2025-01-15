#ifndef _BADREQUEST_HPP
# define _BADREQUEST_HPP

#include "ARequest.hpp"

class BADRequest : public ARequest {
	private:
	unsigned int _code;

public:
	std::string generateResponse(std::vector<Server> & servers);

	BADRequest(void);
	BADRequest(unsigned int code);
	BADRequest(BADRequest const& other);
	~BADRequest(void);
	BADRequest& operator=(BADRequest const& other);
};

#endif // !_BADREQUEST_HPP

