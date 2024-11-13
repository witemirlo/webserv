#include <cstddef>
#include <fstream>
#include <sstream>
// #include <stdexcept>
#include <string>
#include <vector>


#include <iostream> // DEBUG

static std::string get_buffer(std::string const& path)
{
	std::string  buffer, line, token;
	std::fstream file(path.c_str());

	if (!file.is_open())
		return buffer;

	while (std::getline(file, line)) {
		std::istringstream stream(line);
		while (true) {
			stream >> token;
			if (token.empty())
				break;
			buffer += token;
			buffer += ' ';
			token.clear();
		}
		buffer.back() = '\n';
	}

	file.close();

	return buffer;
}


std::vector<std::string> split_config_file(std::string const& path)
{
	std::vector<std::string> container;
	std::string              buffer;
	std::size_t              i, next;

	buffer = get_buffer(path);
	// if (buffer.size() == 0)
	// std::cout << __FILE__ << ": " << __LINE__ << ": \n" << buffer;
	// std::cout.flush();

	i = 0;
	while (buffer[i]) {
		// std::cout << __FILE__ << ": " << __LINE__ << "| " << i << "->" << buffer[i] << std::endl;
		// i = buffer.find("[server]\n", i);
		// next = buffer.find("[server]\n", i + 1);
		// container.push_back(buffer.substr(i, next));
		// i = next;
		i = buffer.find("\n", i);
		next = buffer.find("\n", i + 1);
		std::cout << buffer.substr(i, next);
		i = next;
	}

	// for (std::vector<std::string>::iterator it = container.begin(); it != container.end(); it++)
	// 	std::cout << *it << "\n\n";

	return container;
}

int main()
{
	split_config_file("config.conf");
}