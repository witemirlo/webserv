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

std::vector<Listener> setup(void)
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

int get_all_sockets(struct pollfd ** fds, std::vector<Listener> & sockets)
{
	size_t total_sz = 0;
	for (size_t i = 0; i < sockets.size(); i++)
		total_sz += sockets[i].getNumberofSockets();
	
	struct pollfd * skts = new struct pollfd [total_sz];
	struct pollfd * set_of_fds;
	int fds_in_set;
	int total_fds = 0;
	for (size_t i = 0; i < sockets.size(); i++)
	{
		fds_in_set = sockets[i].getSockets(&set_of_fds);
		for (int j = 0; j < fds_in_set; j++)
			skts[j + total_fds] = set_of_fds[j];
		total_fds += fds_in_set;
		delete [] set_of_fds;
	}
	*fds = skts;
	return (total_sz);
}

int where_is(int fd, std::vector<Listener> & sockets)
{
	for (size_t i = 0; i < sockets.size(); i++)
	{
		int ind = sockets[i].is_fd_here(fd);
		if (ind == FD_NOT_HERE)
			continue ;
		if (ind == FD_IS_LISTENER)
			return ((i + 1) * - 1);
		else
			return (i + 1);
	}
	return (0);
}

void accept_new_conn(Listener & listener, int fd)
{
	int new_fd = accept(fd, NULL, NULL);
	
	if (new_fd == -1)
	{
		std::cerr << "Could not accept new connection" << std::endl;
		exit (EXIT_FAILURE);
	}

	listener.addSocket(new_fd);
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

	std::vector<Listener> sockets = setup();
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
					accept_new_conn(sockets[(loc * -1) - 1], my_fds[i].fd);
				else if (loc > 0)
					false_http(sockets[loc - 1], my_fds[i].fd);
				// std::cerr << my_fds[i].fd << " is ready to read" << std::endl;
			}
		}
	}

	delete [] my_fds;
	return EXIT_SUCCESS;
	if (!argc && !argv[0])
		return 1;
}
