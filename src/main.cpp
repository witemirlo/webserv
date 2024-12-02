#include <cstdlib>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "Listener.hpp"
#include "Server.hpp"
#include "delimiter.hpp"
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
			(*listener_socks.rbegin()).addServer(my_servers[i]);
		}
	}
	return (listener_socks);
}

void respond_http(Listener & listener, int fd)
{
	std::string returned = listener.respondTo(fd);
	const char *response = returned.c_str();
	int sent = 0;
	int len = strlen(response);
	int iter;

	while (sent < len)
	{
		iter = send(fd, response + sent, len - sent, 0);
		if (iter == -1)
		{
			std::cerr << RED "Error: " NC "an error ocurred while sending" << std::endl;
			exit(EXIT_FAILURE);
		}
		sent += iter;
	}
}

int false_http(Listener & listener, int fd)
{
	char buffer[128]; //TODO:  hay que verlo
	std::memset(buffer, 0, sizeof buffer);
	int bytes = recv(fd, buffer, sizeof buffer - 1, 0);

	if (bytes == -1)
	{
		std::cout << "Error while reciving" << std::endl;
		exit (EXIT_FAILURE);
	}

	if (bytes == 0)
	{
		listener.deleteFd(fd);
		return (0);
	}

	int status = listener.updateRequest(fd, std::string(buffer));
	
#ifdef DEBUG
	switch (status)
	{
	case INIT:
		std::cout << GREEN "INIT" NC << std::endl;
		break;
	case HEADERS:
		std::cout << YELLOW "HEADERS" NC << std::endl;
		break;	
	case BODY:
		std::cout << MAGENTA "BODY" NC << std::endl;
		break;
	case END:
		std::cout << CYAN "END" NC << std::endl;
		break;
	default:
		std::cout << "THE FUCK?" << std::endl;
	}
#endif

	if (status == END)
		listener.setFdToWrite(fd);
	return (status);
}

int main(int argc, char* argv[])
{
	if (argc > 2) {
		std::cerr << "Error: bad input" << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<std::map<std::string, std::string> > server_config;
	server_config = get_config_data((argc == 2) ? argv[1] : DEFAULT_CONFIG_PATH);

	// NOTE: debug, print returned container---------------------------------------------------------------------------------------------
	std::map<std::string, std::string>::iterator i;
	std::vector<std::map<std::string, std::string> >::iterator it;
	std::size_t n = 0;
	for (it = server_config.begin(); it < server_config.end(); it++) {
		std::cerr << __FILE__ << ": " << __LINE__ << " | server[" << n << "]: " << std::endl;
		for (i = it->begin(); i != it->end(); i++) {
			std::cerr << __FILE__ << ": " << __LINE__ << " | [key: " << i->first << ", value: ";
			for (std::size_t j = 0; i->second[j]; j++) {
				switch (i->second[j]) {
				case STX:
					std::cout << GREEN "[STX]" NC;
					break;
				case ETX:
					std::cout << GREEN "[ETX]" NC;
					break;
				case US:
					std::cout << YELLOW "[US]" NC;
					break;
				default:
					std::cout << i->second[j];
					break;
				}
			}
			std::cerr << "]" << std::endl;
		}
		n++;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------

	std::vector<Listener> sockets = setup(server_config);
	struct pollfd *my_fds;
	int fd_num;

	while (42)
	{
		fd_num = get_all_sockets(&my_fds, sockets);

		int n_events = poll(my_fds, fd_num, -1);
		if (n_events == -1)
		{
			std::cerr << RED "An error in poll happened" NC << std::endl; //TODO: error format
			return EXIT_FAILURE;
		}
	
		for (int i = 0; i < fd_num; i++)
		{
			if (my_fds[i].revents & POLLIN)
			{
				int loc = where_is(my_fds[i].fd, sockets);
				if (loc < 0)
					accept_new_conn(sockets[WH_NEGATIVE(loc)], my_fds[i].fd);
				else if (loc > 0)
					false_http(sockets[WH_POSITIVE(loc)], my_fds[i].fd);
			}
			else if (my_fds[i].revents & POLLOUT)
			{
				int loc = where_is(my_fds[i].fd, sockets);
				respond_http(sockets[WH_POSITIVE(loc)], my_fds[i].fd);
			}
		}
	}

	delete [] my_fds;
	return EXIT_SUCCESS;
	if (!argc && !argv[0])
		return 1;
}
