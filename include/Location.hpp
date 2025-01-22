#ifndef LOCATION_HPP
# define LOCATION_HPP
# define CGI "/usr/bin/php-cgi"

# include "Server.hpp"

# include <sys/types.h>
# include <sys/socket.h>
# include <poll.h>

class Location : public Server {
private:
	unsigned int _deepness;
	static const std::string prohibited_rules[];
	static const std::pair<std::string, std::string> _file_types[];

	std::map<std::string, std::string> getCgiHeaders(std::string const& body) const;
	std::string getFileType(std::string const& file) const;
	std::string getBody(std::string const& uri) const;
	std::string getBodyError(int status_code) const;
	std::string getGmtTime(void) const;
	std::string getContentType(std::string const& path) const;
	std::string getPathTo(std::string const& uri, bool index) const;
	std::string readFile(std::string const& path) const;
	std::string autoIndex(std::string const& uri) const;

	std::string getStatusLine(void) const;
	int getStatusCode(void) const;


	std::string CGIget(std::string const& file, std::string const& query) const;
	std::string CGIpost(std::string const& file, std::string const& body, std::string const& type, std::string const& len) const;
	void callGETcgi(std::string const& file, std::string const& query) const;
	void callPOSTcgi(std::string const& file, std::string const& type, std::string const& len) const;

	std::string getHttpMessage(int code) const;

	bool copy_file(std::string const& body, std::string const& dest) const;

public:
	Location();
	~Location();
	Location(const Location &other);
	Location(Server const& o, std::string const & config, std::string const & my_path);

	std::string getStatusLine(unsigned int code) const;
	std::string getHeaders(std::string const& body, std::string const& uri, int status_code) const;

	Location &operator=(const Location &other);
	bool operator>(const Location & other) const;
	bool operator<(const Location & other) const;
	bool operator>=(const Location & other) const;
	bool operator<=(const Location & other) const;

	std::string responseGET(std::string const& uri, std::string const& query) const;
	std::string responseGET(unsigned int error_code) const;
	std::string responseGET(unsigned int error_code, std::string const& uri) const;
	std::string responseDELETE(std::string const& uri, std::string const& query) const;
	std::string responsePOST(std::string const& uri, std::string const& msg, std::string const& type, std::string const& len) const;
};

std::string read_cgi_response(int fd);

#endif
