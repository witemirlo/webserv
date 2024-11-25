#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "Server.hpp"

class Location : public Server
{
private:
	unsigned int _deepness;
	// Location();
	Location &operator=(const Location &other);

public:
	Location(const Location &other);

	bool operator>(const Location & other) const;
	std::string getPathTo(std::string const & uri) const; 

	Location(Server const& o, std::string const & config) : Server(o) {/*
	Parsear el config
	Llamar a procRule con cada paso
	OJO a futuro con rules asociadas solo a los servidores
	*/};
	~Location();
};

#endif
