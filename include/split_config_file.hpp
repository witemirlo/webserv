#ifndef _SPLIT_CONFIG_FILE_HPP
# define _SPLIT_CONFIG_FILE_HPP

#include <map>
#include <string>
#include <vector>

std::vector<std::map<std::string, std::string> > split_config_file(std::string const& path);

#endif