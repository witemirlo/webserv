#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#ifndef DEFAULT_CONFIG_PATH
# define DEFAULT_CONFIG_PATH "config.toml"
#endif

int main(int argc, char* argv[])
{
	if (argc > 2) {
		std::cerr << "Error: bad input" << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<std::string> server_config;
	server_config = split_config_file((argc == 2) ? argv[1] : DEFAULT_CONFIG_PATH);

	return EXIT_SUCCESS;
}
