#include <algorithm>
#include <cctype>
#include <cstddef>
#include <endian.h>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>


#include <iostream> // DEBUG

#define STX 2  // Start TeXt
#define ETX 3  // End TeXt
#define US  31 // Unit Separator

std::string trim(std::string const& str)
{
	std::size_t start, end;
	std::string buffer;

	start = 0;
	while (str.size() && (str[start] == ' ' || str[start] == '\t'))
		start++;

	end = str.size() - 1;
	while (end && (str[end] == ' ' || str[end] == '\t' || str[end] == '\n')) {
		end--;
	}

	end = end - start;
	buffer = str.substr(start, end + 1);
	// buffer = buffer.substr(0, buffer.find('#'));
	// if (buffer.size())
	// 	buffer.push_back('\n');

	return buffer;
}

int get_line_case(char token, bool open_quote)
{
	if (token == '\"')
		return 3;

	if (open_quote)
		return 0;

	if (token == '[')
		return 1;

	if (token == ']')
		return 2;

	if (token == '#')
		return 4;

	if (token == ',')
		return 5;

	if (token == '\n')
		return 6;

	return 0;
}

std::string get_line(std::istream& stream, std::string& buffer)
{
	std::string      line;
	std::stack<char> container;
	std::size_t      i;
	bool             open_brace, open_quote;

	i = 0;
	open_brace = false;
	open_quote = false;
	do {
		if (!buffer[i]) {
			buffer.clear();
			std::getline(stream, buffer);
			if (buffer.empty())
				throw std::invalid_argument("syntax error: missing operator");
			i = 0;
		}

		switch (get_line_case(buffer[i], open_quote)) {
		case 1:
			open_brace = true;
			container.push('[');
			line += STX;
			break;
		case 2:
			if (container.size() && container.top() != '[')
				throw std::invalid_argument("syntax error: missing '['");
			container.pop();
			if (container.size() == 0)
				open_brace = false;
			line += ETX;
			break;
		case 3:
			if (container.size() && container.top() == '\"') {
				container.pop();
				open_quote = false;
			} else {
				container.push('\"');
				open_quote = true;
			}
			break;
		case 4:
			i = buffer.length() - 1;
			break;
		case 5:
			line += US;
			break;
		case 6:
			break;
		default:
			if (open_quote || !std::isspace(buffer[i]))
				line += buffer[i];
			break;
		}

		i++;
	} while (container.size() != 0 || open_brace || open_quote || buffer[i]);

	line = trim(line);
	return line;
}

bool get_case(std::string const& str)
{
	std::string buffer = trim(str);

	// std::cerr << __FILE__ << ": " << __LINE__ << ": " << buffer;
	
	if (buffer.size() == 0 || buffer[0] == '#')
		return true;

	if (buffer == "[server]")// ERROR: [   server    ]
		return true;

	if (buffer.find('=') == std::string::npos)
		return false;

	if (buffer.find('=') == buffer.rfind('='))
		return true;

	return false;
}

std::vector<std::string> get_raw_file(std::string const& path)
{
	std::vector<std::string> container;
	std::string              buffer, line;
	std::fstream             file(path.c_str());

	if (!file.is_open())
		return container;

	while (std::getline(file, buffer)) {
		// buffer = trim(buffer);

		if (get_case(buffer))
			line = get_line(file, buffer);
		else
			throw std::invalid_argument("syntax error");

		if (line.size()) {
			container.push_back(line);
		}
		buffer.clear();
	}

	file.close();
	return container;
}

std::vector<std::map<std::string, std::string> > split_config_file(std::string const& path)
{
	std::vector<std::map<std::string, std::string> > final;
	std::vector<std::string> v_container;
	std::vector<std::string>::iterator    v_it;
	std::map<std::string, std::string>    m_container;
	std::map<std::string, std::string>::iterator    m_it;
	std::string              key, value;

	v_container = get_raw_file(path);
	// TODO: funcion para dividir en contenedores por cada [server], para luego que cada uno corresponda al que toca
	v_it = v_container.begin();
	while (v_it != v_container.end()) {
		key = v_it->substr(0, v_it->find('='));
		value = v_it->substr(v_it->find('=') + 1);


		m_it = m_container.find(key);
		if (m_it != m_container.end())
			throw std::invalid_argument("repeated value"); // TODO: poner la clave repetida
		m_container[key] = value;
		v_it++;

	std::cerr << __FILE__ << ": " << __LINE__ << ": [key:" << key << ", value: " << value << "]" << std::endl;
	}

	return final;
}

int main()
{
	split_config_file("config.conf");
}