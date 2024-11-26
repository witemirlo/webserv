#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <string>
# include <vector>

# include "colors.hpp"

# define DEF_PORT "8080"
# define DEF_HOST "localhost"

class Server
{
private:
	std::map<std::string, Location> _locations;

protected:
	std::string	_listen;
	std::string _server_name;
	std::string _root;
	std::vector<std::string > _index;
	static const std::string rules[];
	static void (Server::* const setters [])(std::string const &);

public:
	Server(std::map<std::string, std::string> & config);
	void procRule(std::string const &what, std::string const &to_set);

//	SETTER
	void setServerName(std::string const &server_name);
	void setListen(std::string const &listen);
	void setRoot(std::string const &root);
	void setIndex(std::string const &index);

//	GETTER
	std::string &getServerName(void);
	std::string &getListen(void);
	std::string &getRoot(void);
	std::vector<std::string> &getIndex(void);

//	OCCF
	Server();
	Server(const Server &other);
	Server &operator=(const Server &other);
	~Server();
};

#endif
