#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "Server.hpp"

class Location : public Server
{
private:
	unsigned int _deepness;
	static const std::string prohibited_rules[];
	static const std::map<std::string, std::string> _file_types[];

	std::string getFileType(std::string const& file) const;
	std::string getBody(std::string const& uri) const;
	std::string getPathTo(std::string const& uri) const;
	std::string readFile(std::string const& path) const;
	std::string autoIndex(std::string const& path) const;

	int getStatusCode(void) const;

	std::string getHeaders(std::string const& body) const;

	std::string CGIget(std::string const& file, std::string const& query); //TODO: path_info

public:
	Location();
	~Location();
	Location(const Location &other);
	Location(Server const& o, std::string const & config, std::string const & my_path);

	Location &operator=(const Location &other);
	bool operator>(const Location & other) const;
	bool operator<(const Location & other) const;
	bool operator>=(const Location & other) const;
	bool operator<=(const Location & other) const;

	std::string responseGET(std::string const& uri) const;
};

#endif
