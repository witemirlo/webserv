#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


#include <iostream>

std::string get_buffer(std::string const& path)
{
	std::string  buffer, line, token;
	std::fstream file(path.c_str());

	if (!file.is_open())
		return buffer;

	// line puede ser un istringstream??
	while (std::getline(file, line)) {
		std::istringstream stream(line);
		while (true) {
			stream >> token;
			if (token.empty())
				break;

			// std::cout << __FILE__ << ": " << __LINE__ << ": " << token << std::endl;

			buffer += token;
			buffer += ' ';
			token.clear();
		}
		buffer[buffer.size() - 1] = '\n';
	}

	file.close();

	return buffer;
}

std::vector<std::string> split_config_file(std::string const& path)
{
	std::vector<std::string> container;
	std::string              buffer;

	buffer = get_buffer(path);
	std::cout << buffer;

	return container;
}

int main()
{
	split_config_file("config.toml");
}