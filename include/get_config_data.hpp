#ifndef _GET_CONFIG_DATA_HPP
# define _GET_CONFIG_DATA_HPP

# ifndef DEFAULT_CONFIG_PATH
#  define DEFAULT_CONFIG_PATH "server.conf"
# endif

# include <map>
# include <string>
# include <vector>

std::vector<std::map<std::string, std::string> > get_config_data(std::string const&);

std::string trim(std::string const&);

#endif
