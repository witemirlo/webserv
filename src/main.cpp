#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "get_config_data.hpp"

int main(int argc, char* argv[])
{
	if (argc > 2) {
		std::cerr << "Error: bad input" << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<std::map<std::string, std::string> > server_config;
	server_config = get_config_data((argc == 2) ? argv[1] : DEFAULT_CONFIG_PATH);

	return EXIT_SUCCESS;
}
