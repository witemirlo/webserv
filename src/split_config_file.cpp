#include <algorithm>
#include <cctype>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <stdexcept>
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

std::string trim(std::string const& str)
{
	std::size_t start, end;
	std::string buffer;

	start = 0;
	while (str.size() && std::isspace(str[start]))
		start++;

	end = str.size() - 1;
	while (end && std::isspace(str[end]))
		end--;
	
	buffer = str.substr(start, end);
	return buffer;
}

int get_case(std::string const& str)
{
	std::string buffer = trim(str);

	// TODO: mirar las lineas que solo tienen comentarios
	if (buffer.size() == 0)
		return 2;

	if (buffer.find('=') == buffer.rfind('='))
		return 0;

	if (buffer == "[server]\n")
		return 1;

	return 3;
}

std::string get_line(std::istream& stream, std::string const& buffer)
{
	std::string line;

	return line;
}

// poner en una linea la clave valor, si tiene '[' leer hasta que encuentre ']'
std::vector<std::string> get_raw_file(std::string const& path)
{
	std::vector<std::string> container;
	std::string              buffer, line;
	std::fstream             file(path.c_str());
	std::size_t              i;

	if (!file.is_open())
		return container;

	while (std::getline(file, buffer)) {
		line.clear();
		switch (get_case(buffer)) {
		case 0:		
			line = get_line(file, buffer);
			break;
		case 1:		
			line = trim(buffer);
			break;
		case 2:
			continue;
		default:		
			throw std::invalid_argument("invalid argument");
		}
		if (line.size() )
	}

	file.close();
	return container;
}

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