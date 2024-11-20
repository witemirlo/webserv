#ifndef SOCKET_MANAGEMENT_HPP
# define SOCKET_MANAGEMENT_HPP
# define WH_POSITIVE(i) i - 1 
# define WH_NEGATIVE(i) (i * -1) - 1

# include <vector>

# include "colors.hpp"
# include "Listener.hpp"

int where_is(int fd, std::vector<Listener> & sockets);
void accept_new_conn(Listener & listener, int fd);
int get_all_sockets(struct pollfd ** fds, std::vector<Listener> & sockets);

#endif
