#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

#include "Server.hpp"
#include "Listener.hpp"
#include "get_config_data.hpp"
#include "socket_management.hpp"

std::vector<Listener> setup(std::vector<std::map<std::string, std::string> > & config)
{
	std::vector<Server> my_servers;

	for (size_t i = 0; i < config.size(); i++)
	{
		my_servers.push_back(Server(config[i]));
	}
	std::map<std::string, int> host_index; //TODO: revisar, se puede hacer + limpio
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

void false_http(Listener & listener, int fd)
{
	char buffer[128]; //hay que verlo
	std::memset(buffer, 0, sizeof buffer);
	int bytes = recv(fd, buffer, sizeof buffer - 1, 0);

	if (bytes == -1)
	{
		std::cout << "Error while reciving" << std::endl;
		exit (EXIT_FAILURE);
	}

	if (bytes == 0)
		listener.deleteFd(fd);

	std::cout << "Hi, im fd: " << fd << " and I proudly say: " << buffer << std::endl;
}

int main(int argc, char* argv[])
{
	if (argc > 2) {
		std::cerr << "Error: bad input" << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<std::map<std::string, std::string> > server_config;
	server_config = get_config_data((argc == 2) ? argv[1] : DEFAULT_CONFIG_PATH);

	std::vector<Listener> sockets = setup(server_config);
	struct pollfd *my_fds;
	int fd_num;

	while (42) //TODO: no es non blocking
	{
		fd_num = get_all_sockets(&my_fds, sockets);

		int n_events = poll(my_fds, fd_num, -1);
		if (n_events == -1)
		{
			std::cerr << RED "An error in poll happened" NC << std::endl;
			return EXIT_FAILURE;
		}
	
		for (int i = 0; i < fd_num; i++)
		{
			if (my_fds[i].revents & POLLIN)
			{
				int loc = where_is(my_fds[i].fd, sockets);
				// std::cerr << my_fds[i].fd << " is in " << loc << std::endl;
				if (loc < 0)
					accept_new_conn(sockets[WH_NEGATIVE(loc)], my_fds[i].fd);
				else if (loc > 0)
					false_http(sockets[WH_POSITIVE(loc)], my_fds[i].fd);
				// std::cerr << my_fds[i].fd << " is ready to read" << std::endl;
			}
		}
	}

	delete [] my_fds;
	return EXIT_SUCCESS;
	if (!argc && !argv[0])
		return 1;
}
