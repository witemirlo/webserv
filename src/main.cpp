#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>

#include "Server.hpp"
#include "Listener.hpp"

std::vector<Listener> setup()
{
	std::vector<Server> my_servers;
	my_servers.push_back(Server(std::string("listen = 4143\nserver_name = mundo.com")));
	my_servers.push_back(Server(std::string("listen = 1024\nserver_name = aver")));
	my_servers.push_back(Server(std::string("listen = 4143\nserver_name = psacrist")));

	std::map<std::string, int> host_index; //revisar, se puede hacer + limpio
	std::vector<Listener> listener_socks;

	for (size_t i = 0; i < my_servers.size(); i++)
	{
		try
		{
			int ind = host_index.at(my_servers[i].getListen());
			listener_socks[ind].addServer(my_servers[i]);
		}
		catch(const std::exception& e)
		{
			listener_socks.push_back(Listener(my_servers[i].getListen()));
			host_index[my_servers[i].getListen()] = host_index.size();
		}
	}
	return (listener_socks);
}

int main(int argc, char* argv[])
{
	std::vector<Listener> sockets = setup();
	struct pollfd *my_fds;
	int fd_num;

	sockets[0].addSocket(4);
	sockets[0].addSocket(5);
	sockets[0].addSocket(6);
	sockets[0].addSocket(7);
	while (42)
	{
		fd_num = sockets[0].getSockets(&my_fds);
		for (int i = 0; i < fd_num; i++)
		{
			std::cout << "I have a socket : " << my_fds[i].fd << std::endl;
		}
		delete [] my_fds;
		break;
	}

	return EXIT_SUCCESS;
	if (!argc && !argv[0])
		return 1;
}
