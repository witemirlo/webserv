#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>

# define DEF_PORT "80"
# define DEF_HOST "localhost"

class Server
{
private:
	std::string	_listen;
	std::string _server_name;
	static const std::string rules[];
	static void (Server::* const setters [])(std::string &);
public:
	Server(std::string const &config);
	void procRule(std::string &rule);

//	SETTER
	void setServerName(std::string &server_name);
	void setListen(std::string &listen);

//	GETTER
	std::string &getServerName(void);
	std::string &getListen(void);

//	OCCF
	Server();
	Server(const Server &other);
	Server &operator=(const Server &other);
	~Server();
};

# ifndef COLOR_DEF
#  define COLOR_DEF
#  define NC "[0m"
#  define GREEN "[38;5;40m"
#  define YELOW "[38;5;220m"
#  define RED "[38;5;124m"
#  define BLUE "[38;5;32m"
#  define ORAN "[38;5;202m"
#  define PURP "[38;5;91m"
# endif

#endif
