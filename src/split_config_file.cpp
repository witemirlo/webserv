#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>


#include <iostream> // DEBUG

// static std::vector<std::string> get_raw_file(std::string const& path)
// {
// 	std::vector<std::string> container;
// 	std::string              buffer, line, token;
// 	std::fstream             file(path.c_str());

// 	if (!file.is_open())
// 		return container;

// 	while (std::getline(file, line)) {
// 		std::istringstream stream(line);
// 		while (true) {
// 			stream >> token;
// 			if (token.empty() || token[0] == '#')
// 				break;
// 			buffer += token + ' ';
// 			token.clear();
// 		}
// 		if (buffer.size()) {
// 			buffer.back() = '\n';
// 			container.push_back(buffer);
// 			buffer.clear();
// 		}
// 	}

// 	file.close();
// 	return container;
// }

// static std::vector<std::string> split_raw_file(std::vector<std::string> const& other)
// {
// 	std::vector<std::string>::const_iterator current, next;
// 	std::vector<std::string>                 container;
// 	std::string                              buffer;

// 	current = std::find(other.begin(), other.end(), "[server]\n");
// 	if (current != other.begin())
// 		return container;
	
// 	while (true) {
// 		current = std::find(current, other.end(), "[server]\n");
// 		next = std::find(current + 1, other.end(), "[server]\n");
		
// 		if (current == other.end())
// 			break;

// 		current++;
// 		while (current != next) {
// 			buffer += *current;
// 			current++;
// 		}

// 		if (buffer.size()) {
// 			container.push_back(buffer);
// 			buffer.clear();
// 		} else {
// 			break;
// 		}
// 	}

// 	return  container;
// }

// poner en una linea la clave valor, si tiene '[' leer hasta que encuentre ']'

std::vector<std::string> split_config_file(std::string const& path)
{
	std::vector<std::string> container;
	std::string              buffer;

	container = get_raw_file(path);
	container = split_raw_file(container);
	// container = delete_quotes(container);

	for (std::vector<std::string>::iterator it = container.begin(); it != container.end(); it++)
		std::cout << *it << "---\n";

	return container;
}

int main()
{
	split_config_file("config.conf");
}