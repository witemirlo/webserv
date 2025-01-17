#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <string>
# include <vector>
# include <algorithm>

# include "colors.hpp"
# include "delimiter.hpp"

# define DEF_PORT "8080"
# define DEF_HOST "localhost"
# define DEF_ROOT "/var"
# define DEF_BODY_SIZE 5000000
# define DEF_METHODS "GET\037POST\037DELETE"

// DEFAULT ERROR PAGES -------------------
# define DEF_100_FILE DEF_ROOT "/100.html"
# define DEF_101_FILE DEF_ROOT "/101.html"

# define DEF_200_FILE DEF_ROOT "/200.html"
# define DEF_201_FILE DEF_ROOT "/201.html"
# define DEF_202_FILE DEF_ROOT "/202.html"
# define DEF_203_FILE DEF_ROOT "/203.html"
# define DEF_204_FILE DEF_ROOT "/204.html"
# define DEF_205_FILE DEF_ROOT "/205.html"
# define DEF_206_FILE DEF_ROOT "/206.html"

# define DEF_300_FILE DEF_ROOT "/300.html"
# define DEF_301_FILE DEF_ROOT "/301.html"
# define DEF_302_FILE DEF_ROOT "/302.html"
# define DEF_303_FILE DEF_ROOT "/303.html"
# define DEF_304_FILE DEF_ROOT "/304.html"
# define DEF_305_FILE DEF_ROOT "/305.html"
# define DEF_307_FILE DEF_ROOT "/307.html"
# define DEF_308_FILE DEF_ROOT "/308.html"

# define DEF_400_FILE DEF_ROOT "/400.html"
# define DEF_401_FILE DEF_ROOT "/401.html"
# define DEF_402_FILE DEF_ROOT "/402.html"
# define DEF_403_FILE DEF_ROOT "/403.html"
# define DEF_404_FILE DEF_ROOT "/404.html"
# define DEF_405_FILE DEF_ROOT "/405.html"
# define DEF_406_FILE DEF_ROOT "/406.html"
# define DEF_407_FILE DEF_ROOT "/407.html"
# define DEF_408_FILE DEF_ROOT "/408.html"
# define DEF_409_FILE DEF_ROOT "/409.html"
# define DEF_410_FILE DEF_ROOT "/410.html"
# define DEF_411_FILE DEF_ROOT "/411.html"
# define DEF_412_FILE DEF_ROOT "/412.html"
# define DEF_413_FILE DEF_ROOT "/413.html"
# define DEF_414_FILE DEF_ROOT "/414.html"
# define DEF_415_FILE DEF_ROOT "/415.html"
# define DEF_416_FILE DEF_ROOT "/416.html"
# define DEF_417_FILE DEF_ROOT "/417.html"
# define DEF_421_FILE DEF_ROOT "/421.html"
# define DEF_422_FILE DEF_ROOT "/422.html"
# define DEF_426_FILE DEF_ROOT "/426.html"

# define DEF_500_FILE DEF_ROOT "/500.html"
# define DEF_501_FILE DEF_ROOT "/501.html"
# define DEF_502_FILE DEF_ROOT "/502.html"
# define DEF_503_FILE DEF_ROOT "/503.html"
# define DEF_504_FILE DEF_ROOT "/504.html"
# define DEF_505_FILE DEF_ROOT "/505.html"
//----------------------------------------


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
	std::string _cgi_extension;
	std::map<std::string, std::string> _redirect;
	size_t _body_size;
	std::vector<std::string> _allow;

	static const std::string rules[];
	static void (Server::* const setters [])(std::string const &);

	std::string _err_tmp;
public:
	Server(std::map<std::string, std::string> & config);
	void procRule(std::string const &what, std::string const &to_set);
	bool isNamed(std::string & name);
	std::vector<std::string> setVector(std::string const & str);
	Location const& getLocation(std::string const& uri) const;
	void callGETcgi(std::string const& file, std::string const& query) const;
	void callPOSTcgi(std::string const& file, std::string const& type, std::string const& len) const;

//	SETTER
	void setServerName(std::string const &server_name);
	void setListen(std::string const &listen);
	void setRoot(std::string const &root);
	void setIndex(std::string const &index);
	void setAutoIndex(std::string const&autoindex);
	void setErrorPages(std::string const &errors);
	void setCGIExtension(std::string const &cgi_extension);
	void setRedirections(std::string const &redirections);
	void setBodySize(std::string const &body_size);
	void setAllowedMethods(std::string const &allowed_methods);

//	GETTER
	std::vector<std::string> const& getServerName(void) const;
	std::string const& getListen(void) const;
	std::string const& getRoot(void) const;
	std::vector<std::string> const& getIndex(void) const;
	bool getAutoIndex(void) const;
	std::string const& getCGIExtension(void) const;

//	OCCF
	Server();
	Server(const Server &other);
	Server &operator=(const Server &other);
	~Server();
};

#endif
