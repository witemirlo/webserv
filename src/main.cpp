#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <map>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "Listener.hpp"
#include "Server.hpp"
#include "delimiter.hpp"
#include "get_config_data.hpp"
#include "socket_management.hpp"

static int signal_num = 0;

void signal_handler(int sig, siginfo_t * info, void * args)
{
	if (info && args)
		signal_num = sig;
}

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
			(*listener_socks.rbegin()).addServer(my_servers[i]);
		}
	}
	return (listener_socks);
}

void close_all(std::vector<Listener> sockets)
{
	for (size_t i = 0; i < sockets.size(); i++)
		sockets[i].closeFds();

	std::cerr << "webserv was closed..." << std::endl;
	exit(EXIT_FAILURE);
}

void pollloop(std::vector<Listener> sockets)
{
	struct pollfd *my_fds;
	int fd_num;

	while (42)
	{
		fd_num = get_all_sockets(&my_fds, sockets);

		int n_events = poll(my_fds, fd_num, -1);
		if (signal_num == SIGINT)
		{
			delete [] my_fds;
			close_all(sockets);
		}
		if (n_events == -1)
		{
			std::cerr << RED "Error: " NC << std::strerror(errno) << std::endl;
			exit (EXIT_FAILURE);
		}

		for (int i = 0; i < fd_num; i++)
		{
			if (my_fds[i].revents & POLLIN)
			{
				int loc = where_is(my_fds[i].fd, sockets);
				if (loc < 0)
					accept_new_conn(sockets[WH_NEGATIVE(loc)], my_fds[i].fd);
				else if (loc > 0)
					sockets[WH_POSITIVE(loc)].readFrom(my_fds[i].fd);
			}
			else if (my_fds[i].revents & POLLOUT)
			{
				int loc = where_is(my_fds[i].fd, sockets);
				sockets[WH_POSITIVE(loc)].respondTo(my_fds[i].fd);
			}
		}
		delete [] my_fds;
	}
}

int main(int argc, char* argv[])
{
	if (argc > 2) {
		std::cerr << "Error: bad input" << std::endl;
		return EXIT_FAILURE;
	}

	struct sigaction sa;
	memset(&sa, 0, sizeof (sa));
	sa.sa_sigaction = &signal_handler;
	sigaction(SIGINT, &sa, NULL);

	std::vector<std::map<std::string, std::string> > server_config;
	server_config = get_config_data((argc == 2) ? argv[1] : DEFAULT_CONFIG_PATH);

	// NOTE: debug, print returned container---------------------------------------------------------------------------------------------
	std::map<std::string, std::string>::iterator i;
	std::vector<std::map<std::string, std::string> >::iterator it;
	std::size_t n = 0;
	for (it = server_config.begin(); it < server_config.end(); it++) {
		std::cerr << __FILE__ << ":" << __LINE__ << " | server[" << n << "]: " << std::endl;
		for (i = it->begin(); i != it->end(); i++) {
			std::cerr << __FILE__ << ":" << __LINE__ << " | [key: " << i->first << ", value: ";
			for (std::size_t j = 0; i->second[j]; j++) {
				switch (i->second[j]) {
				case STX:
					std::cerr << GREEN "[STX]" NC;
					break;
				case ETX:
					std::cerr << GREEN "[ETX]" NC;
					break;
				case US:
					std::cerr << YELLOW "[US]" NC;
					break;
				default:
					std::cerr << i->second[j];
					break;
				}
			}
			std::cerr << "]" << std::endl;
		}
		n++;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------

	std::vector<Listener> sockets = setup(server_config);
	pollloop(sockets);

	return EXIT_SUCCESS;
	if (!argc && !argv[0])
		return 1;
}
