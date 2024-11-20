#include "Listener.hpp"
#include "Server.hpp"

#include <vector>
#include <map>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstdlib>

/**
 * Searchs in which Listener object is an fd
 * 
 * @returns a negative integer if the fd is a pasive socket; a positive if not (you can get the index of the Listener object where the fd is using the WH_POSTIVE and WH_NEGATIVE macros)
 */
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

/**
 * Creates an array of struct pollfd of all the sockets of the program
 * 
 * @param sockets a std::vector with all the Listener objects
 * @param fds an address to where store the struct pollfd
 * @return the total number of sockets
 */
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
