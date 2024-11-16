#include <algorithm>
#include <cctype>
#include <cstddef>
#include <fstream>
#include <map>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

#include "delimiter.hpp"

#include <iostream> // DEBUG

enum token_case {
	LITERAL,
	QUOTE,
	OPEN_BRACE,
	CLOSE_BRACE,
	COMMA,
	COMMENT,
	NEW_LINE
};

static std::string
trim(std::string const& str)
{
	std::size_t start, end;
	std::string buffer;

	if (str.size() == 0)
		return str;

	start = 0;
	while (str[start] == ' ' || str[start] == '\t')
		start++;

	end = str.size() - 1;
	while (end != 0 &&
	      (str[end] == ' ' || str[end] == '\t' || str[end] == '\n')) {
		end--;
	}

	end = end - start;
	buffer = str.substr(start, end + 1);

	return buffer;
}

static enum token_case
get_line_case(char token, bool open_quote)
{
	if (token == '\"')
		return QUOTE;

	if (open_quote)
		return LITERAL;

	if (token == '[')
		return OPEN_BRACE;

	if (token == ']')
		return CLOSE_BRACE;

	if (token == '#')
		return COMMENT;

	if (token == ',')
		return COMMA;

	if (token == '\n')
		return NEW_LINE;

	return LITERAL;
}

static std::string
get_line(std::istream& stream, std::string& buffer)
{
	std::stack<char> container;
	std::string      line;
	std::size_t      i;
	bool             open_brace, open_quote;

	open_brace = false;
	open_quote = false;
	i          = 0;
 	while (container.size() != 0 || open_brace || open_quote || buffer[i]) {
		if (!buffer[i]) {
			buffer.clear();
			std::getline(stream, buffer);
			if (buffer.empty())
				throw std::invalid_argument("syntax error: missing operator");
			i = 0;
		}

		switch (get_line_case(buffer[i], open_quote)) {
		case OPEN_BRACE:
			open_brace = true;
			container.push('[');
			line += STX;
			break;
		case CLOSE_BRACE:
			if (container.size() != 0 && container.top() != '[')
				throw std::invalid_argument("syntax error: missing '['");
			container.pop();
			if (container.size() == 0)
				open_brace = false;
			line += ETX;
			break;
		case QUOTE:
			if (container.size() != 0 && container.top() == '\"') {
				container.pop();
				open_quote = false;
			} else {
				container.push('\"');
				open_quote = true;
			}
			break;
		case COMMENT:
			i = buffer.length() - 1;
			break;
		case COMMA:
			line += US;
			break;
		case NEW_LINE:
			break;
		default:
			if (open_quote || !std::isspace(buffer[i]))
				line += buffer[i];
			break;
		}
		i++;
	}

	line = trim(line);
	return line;
}

static bool
get_case(std::string const& str)
{
	std::string buffer(str);
	buffer.erase(std::remove_if(buffer.begin(), buffer.end(), ::isspace), buffer.end());
	
	if (buffer.size() == 0 || buffer[0] == '#')
		return true;

	if (buffer == "[server]")
		return true;

	if (buffer.find('=') == std::string::npos)
		return false;

	if (buffer.find('=') == buffer.rfind('='))
		return true;

	return false;
}

static std::vector<std::string>
get_raw_file(std::string const& path)
{
	std::vector<std::string> container;
	std::string              buffer, line;
	std::fstream             file(path.c_str());

	if (!file.is_open())
		throw std::invalid_argument("failed to open config file");

	while (std::getline(file, buffer)) {
		if (get_case(buffer))
			line = get_line(file, buffer);
		else
			throw std::invalid_argument("syntax error");

		if (line.size() != 0)
			container.push_back(line);

		buffer.clear();
	}

	file.close();
	return container;
}

static std::vector<std::map<std::string, std::string> >
split_raw_file(std::vector<std::string> const& raw_file)
{
	std::vector<std::map<std::string, std::string> > splited;
	std::vector<std::string>::const_iterator         it;
	std::map<std::string, std::string>               map;
	std::string                                      key, value;

	it = raw_file.begin();
	if (*it != ("\002server\003"))
		throw std::invalid_argument("content outside [server] definition");
	it++;
	while (it != raw_file.end()) {
		if (*it == "\002server\003") {
			splited.push_back(map);
			map.clear();
			it++;
			continue;
		} 

		key = it->substr(0, it->find('='));
		value = it->substr(it->find('=') + 1);

		if (map.find(key) != map.end())
			throw std::invalid_argument("repeated key");

		map[key] = value;
		it++;
	}

	splited.push_back(map);
	return splited;
}

std::vector<std::map<std::string, std::string> >
split_config_file(std::string const& path)
{
	std::vector<std::map<std::string, std::string> > final;
	std::vector<std::string>                         raw_file;

	raw_file = get_raw_file(path);
	final    = split_raw_file(raw_file);

	// NOTE: debug, print returned container---------------------------------------------------------------------------------------------
	std::map<std::string, std::string>::iterator i;
	std::vector<std::map<std::string, std::string> >::iterator it;
	std::size_t n = 0;
	for (it = final.begin(); it < final.end(); it++) {
		std::cerr << __FILE__ << ": " << __LINE__ << " | server[" << n << "]: " << std::endl;
		for (i = it->begin(); i != it->end(); i++)
			std::cerr << __FILE__ << ": " << __LINE__ << " | [key: " << i->first << ", value: " << i->second << "]" << std::endl;
		n++;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------
	return final;
}
