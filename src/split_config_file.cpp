#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

std::string get_buffer(std::string const& path)
{
	std::string  buffer;
	std::fstream file(path.c_str());

	return buffer;
}

std::vector<std::string> split_config_file(std::string const& path)
{
	std::vector<std::string> container;
	std::string              buffer;
	// std::fstream             file(path.c_str());
	// if (!file.is_open())
	    // throw std::invalid_argument("");
	buffer = get_buffer(path);
	// file.close();
	return container;
}