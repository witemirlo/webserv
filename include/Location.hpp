#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "Server.hpp"

class Location : public Server
{
private:
	unsigned int _deepness;
	static const std::string prohibited_rules[];

public:
	Location();
	~Location();
	Location(const Location &other);
	Location(Server const& o, std::string const & config);

	Location &operator=(const Location &other);
	bool operator>(const Location & other) const;
	bool operator<(const Location & other) const;
	bool operator>=(const Location & other) const;
	bool operator<=(const Location & other) const;

	std::string getPathTo(std::string const & uri) const; 

	/*: Server(o) {
	Parsear el config
	Llamar a procRule con cada paso
	OJO a futuro con rules asociadas solo a los servidores
	};*/
};

#endif
