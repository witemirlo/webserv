#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>

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

bool is_listener_fd(int fd, std::vector<Listener> & sockets);

int main(int argc, char* argv[])
{
	std::vector<Listener> sockets = setup();
	struct pollfd *my_fds;
	int fd_num;

	while (42)
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
				//is listner -> accept
				//is other -> read
				std::cerr << my_fds[i].fd << " is ready to read" << std::endl;
			}
			// if (my_fds[i].revents & POLLERR)
			// 	std::cerr << my_fds[i].fd << ": POLLERR" << std::endl;
			// if (my_fds[i].revents & POLLPRI)
			// 	std::cerr << my_fds[i].fd << ": POLLPRI" << std::endl;
			// if (my_fds[i].revents & POLLNVAL)
			// 	std::cerr << my_fds[i].fd << ": POLLNVAL" << std::endl;
		}
		delete [] my_fds;
		break;
	}

	return EXIT_SUCCESS;
	if (!argc && !argv[0])
		return 1;
}
