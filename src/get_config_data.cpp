#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

#include "colors.hpp"
#include "delimiter.hpp"


enum token_case {
	LITERAL,
	QUOTE,
	OPEN_BRACE,
	CLOSE_BRACE,
	COMMA,
	COMMENT,
	NEW_LINE,
	KEY
};

static std::vector<std::string>                         get_file(std::string const&);
static bool                                             get_file_check(std::string const&);
static std::string                                      get_instruction(std::istream&, std::string&);
static enum token_case                                  get_instruction_case(char, bool, bool, std::size_t);
static std::vector<std::map<std::string, std::string> > split_file(std::vector<std::string> const&);
std::string                                             trim(std::string const&);

/**
 * Reads a config file and returns its data formatted.
 *
 * @param path Path of the file, if an error occurs throws and exception .
 * @return A vector that contains a map for each server instruccion founded with all the data extracted.
 */
std::vector<std::map<std::string, std::string> >
get_config_data(std::string const& path)
{
	return split_file(get_file(path));
}

/**
 * Creates a vector with all the data in the file.
 *
 * Puts each instruccion in the config file (server directive or key=value)
 * and formats it in a single line. Then each instrucction is added to the vector.
 *
 * @param path path to the config file.
 * @return A vector with all the instructions of the file.
 */
static std::vector<std::string>
get_file(std::string const& path)
{
	std::vector<std::string> container;
	std::string              buffer, line;
	std::fstream             file(path.c_str());

	if (!file.is_open()) {
		std::cerr << RED "Error:" NC " cannot open file \"" << path << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}

	while (std::getline(file, buffer)) {
		if (get_file_check(buffer))
			line = get_instruction(file, buffer);
		else {
			std::cerr << RED "Error:" NC " syntax error" << std::endl;
			exit(EXIT_FAILURE);
		}

		if (line.size() != 0)
			container.push_back(line);

		buffer.clear();
	}

	file.close();
	return container;
}

/**
 * Checks if a line contains a valid instrucciton.
 *
 * @note in multiline instructions only checks the first line, so the '=' needs to be there.
 * @param str line to check.
 * @return true if it's a valid entry, false otherwise.
 */
static bool
get_file_check(std::string const& str)
{
	std::string buffer(str);
	buffer.erase(std::remove_if(buffer.begin(), buffer.end(), isspace), buffer.end());
	
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

/**
 * Parses a instrucction and puts it in a single line.
 *
 * This function puts all the data of the given instruction and puts it in 
 * a single line. It deletes all whitespace, so if you want to use them
 * (or any other syntax token) in the value field it must be inside double quotes.
 *
 * @param stream file stream for read if there is a multiline instruction.
 * @param buffer original string with the line to parse, it will be modified in a multiline instruction.
 * @return a line with key=[value,...], where '[', ']', and ',' are remplaced with STX, ETX, and US.
 */
static std::string
get_instruction(std::istream& stream, std::string& buffer)
{
	std::stack<char> container;
	std::string      line;
	std::size_t      i, tmp;
	bool             open_brace, open_quote, blank_line, key;

	open_brace = false;
	open_quote = false;
	blank_line = true;
	key        = false;
	i          = 0;
 	while (container.size() != 0 || open_brace || open_quote || buffer[i]) {
		if (!buffer[i]) {
			buffer.clear();
			std::getline(stream, buffer);
			// if (buffer.empty()) {// TODO: antiguo, revisar si todo funciona correctamente
			if (stream.eof()) {
				std::cerr << RED "Error:" NC " syntax error: missing operator" << std::endl;
				exit(EXIT_FAILURE);
			}
			buffer.push_back('\n');
			i = 0;
			blank_line = true;
			if (key && !open_brace) { // TODO: index = [\n
				if (*line.rbegin() == US)
					*line.rbegin() = ETX;
				else
					line += ETX;
			}
			key = false;
		}

		switch (get_instruction_case(buffer[i], open_quote, blank_line, container.size())) {
		case OPEN_BRACE:
			open_brace = true;
			blank_line = false;
			container.push('[');
			line += STX;
			break;

		case CLOSE_BRACE:
			blank_line = false;
			if (container.size() != 0 && container.top() != '[') {
				std::cerr << RED "Error:" NC " syntax error: missing '['" << std::endl;
				exit(EXIT_FAILURE);
			}
			container.pop();
			if (container.size() == 0)
				open_brace = false;
			if (*line.rbegin() == US)
				*line.rbegin() = ETX;
			else
				line += ETX;
			break;

		case QUOTE:
			blank_line = false;
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
			blank_line = false;
			if (*line.rbegin() != US && *line.rbegin() != STX && *line.rbegin() != ETX)
				line += US;
			break;

		case NEW_LINE:
			break;

		case KEY:
			key = true;
			blank_line = false;
			tmp = i + 1;
			line += '=';
			while (buffer[tmp] && (buffer[tmp] == ' ' || buffer[tmp] == '\t')) // TODO: revisar si hay funcion para hacer esto mas limpio
				tmp++;
			if (buffer[tmp] == '\n') {
				std::cerr << RED "Error: " NC "'=' without content" << std::endl;
				exit(EXIT_FAILURE);
			}
			else if (buffer[tmp] != '[')
				line += STX;
			break;

		default:
			if (open_quote || !std::isspace(buffer[i])) {
				line += buffer[i];
				blank_line = false;
			}
			break;
		}

		i++;
	}

	line = trim(line);
	return line;
}

/**
 * Cheks the token type for the get_line() function.
 *
 * @param token the token to analyce.
 * @param open_quote bool know if the token is inside quotes.
 * @param container_size size of the stack with open '['
 * @return a number for the get_line() switch case.
 */
static enum token_case
get_instruction_case(char token, bool open_quote, bool blank_line, std::size_t container_size)
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

	if (!blank_line && (token == ',' || (token == '\n' && container_size != 0)))
		return COMMA;

	if (token == '\n')
		return NEW_LINE;
	
	if (token == '=' && container_size != 0)
		return KEY;

	return LITERAL;
}

/**
 * Creates a vector with a map with data for each server instruccion in the config file.
 *
 * @param raw_file a vector with all the instructions.
 * @return A vector that contains a map for each server instruccion with all the data extracted.
 */
static std::vector<std::map<std::string, std::string> >
split_file(std::vector<std::string> const& raw_file)
{
	std::vector<std::map<std::string, std::string> > container;
	std::vector<std::string>::const_iterator         it;
	std::map<std::string, std::string>               map;
	std::string                                      key, value;

	it = raw_file.begin();
	if (*it != ("\002server\003")) {
		std::cerr << RED "Error:" NC " content outside [server] definition" << std::endl;
		exit(EXIT_FAILURE);
	}
	it++;
	while (it != raw_file.end()) {
		if (*it == "\002server\003") {
			container.push_back(map);
			map.clear();
			it++;
			continue;
		} 

		key = it->substr(0, it->find('='));
		value = it->substr(it->find('=') + 1);
		if (*value.begin() == STX)
			value = value.substr(1, value.size() - 2);

		if (map.find(key) != map.end()) {
			std::cerr << RED "Error:" NC " repeated key \"" << key << "\"" << std::endl;
			exit(EXIT_FAILURE);
		}

		map[key] = value;
		it++;
	}

	container.push_back(map);
	return container;
}

/**
 * Deletes all whitespace at start and end of a line
 *
 * @param str line to trim
 * @return line without the whitespace ant start and end
 */
std::string
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
