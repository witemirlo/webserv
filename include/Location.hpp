#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "Server.hpp"

class Location : public Server
{
private:
	unsigned int _deepness;
	static const std::string prohibited_rules[];

	std::string getBody(std::string const& uri, int& status_code);
	std::string getPathTo(std::string const& uri) const;
	std::string readFile(std::string const& path) const;
	std::string autoIndex(std::string const& path) const;

	int getStatusCode(void) const;

	std::string getHeaders(std::string const& body) const;

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

	std::string responseGET(std::string const& uri);

	/*: Server(o) {
	Parsear el config
	Llamar a procRule con cada paso
	OJO a futuro con rules asociadas solo a los servidores
	};*/
};

#endif
