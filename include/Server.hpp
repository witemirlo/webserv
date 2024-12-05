#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <string>
# include <vector>

# include "colors.hpp"

# define DEF_PORT "8080"
# define DEF_HOST "localhost"
# define DEF_ROOT "/var"
# define DEF_404 "/var/404.html"

class Location;

class Server
{
private:
	std::map<std::string, Location> _locations;

protected:
	std::string _listen;
	std::vector<std::string> _server_name;
	std::string _root;
	std::vector<std::string> _index;
	bool _autoindex;
	std::map<int, std::string> _error_pages;
	static const std::string rules[];
	static void (Server::* const setters [])(std::string const &);

	std::string _err_tmp;
public:
	Server(std::map<std::string, std::string> & config);
	void procRule(std::string const &what, std::string const &to_set);
	bool isNamed(std::string & name);
	std::vector<std::string> setVector(std::string const & str);
	Location const& getLocation(std::string const& uri) const;

//	SETTER
	void setServerName(std::string const &server_name);
	void setListen(std::string const &listen);
	void setRoot(std::string const &root);
	void setIndex(std::string const &index);
	void setAutoIndex(std::string const&autoindex);
	void setErrorPages(std::string const &errors);

//	GETTER
	std::vector<std::string> const& getServerName(void);
	std::string const& getListen(void);
	std::string const& getRoot(void);
	std::vector<std::string> const& getIndex(void);
	bool getAutoIndex(void);

//	OCCF
	Server();
	Server(const Server &other);
	Server &operator=(const Server &other);
	~Server();
};

#endif
