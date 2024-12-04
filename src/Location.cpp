#include "Location.hpp"
#include "delimiter.hpp"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>

const std::string Location::prohibited_rules[] =  {"listen", "server_name", ""};

Location::Location(void)
	: Server()
{
#ifdef DEBUG
	std::cout << GREEN "Location default constructor called" NC << std::endl;
#endif
}

Location::Location(const Location &other)
	: Server(dynamic_cast<Server const&>(other))
{
#ifdef DEBUG
	std::cout << YELLOW "Location copy constructor called" NC << std::endl;
#endif
	*this = other;
}

Location::~Location()
{
#ifdef DEBUG
	std::cout << RED "Location destrucutor called" NC << std::endl;
#endif
}

/**
 * @param o the Server object used as the default configuration of this new location
 * @param config a std::string with the specifics of the configuration of the location
 * @param my_path the uri path of the locations, used to calculate its deepness
 */
Location::Location(Server const& o, std::string const & config, std::string const & my_path)
	: Server(o)
{
#ifdef DEBUG
	std::cout << GREEN "Location constructor called" NC << std::endl;
#endif
	std::string buffer, line, key, value;
	std::size_t it;

	if (config[0] != STX)
    		buffer = config;
	else
    		buffer = config.substr(1, config.size() - 2);

	while (true) {
		it = buffer.find(US);
		if (it == std::string::npos)
			line = buffer;
		else
			line = buffer.substr(0, it);

		key = line.substr(0, line.find('='));
		value = line.substr(line.find('=') + 1, line.size());
		if (value.size() > 1 && *value.rbegin() != '/')
			value.push_back('/');
		/*
		if key en prohibidas
			std::cerr key solo se permite en server
		*/

		procRule(key, value);

		if (it == std::string::npos)
			break;
		else
			buffer = buffer.substr(it + 1);
	}

	if (my_path == "/")
		this->_deepness = 0;
	else
		this->_deepness = std::count(my_path.begin(),  my_path.end(), '/');
	std::cerr << this->_deepness << std::endl;
}

Location &Location::operator=(const Location &other)
{
#ifdef DEBUG
	std::cout << YELLOW "Location copy assignment operator called" NC << std::endl;
#endif
	if (this == &other)
		return (*this);

	Server::operator=(other);
	this->_deepness = other._deepness;
	return (*this);
}

bool Location::operator>(const Location & other) const
{
	return (this->_deepness > other._deepness);
}

bool Location::operator<(const Location & other) const
{
	return (this->_deepness < other._deepness);
}

bool Location::operator>=(const Location & other) const
{
	return (this->_deepness >= other._deepness);
}

bool Location::operator<=(const Location & other) const
{
	return (this->_deepness <= other._deepness);
}

/**
 * Generates the full path to a file
 *
 * @param uri uri to the file
 * @return string with the full path to the file
 */
std::string Location::getPathTo(std::string const& uri) const
{
	return (this->_root + uri);
}

/**
 * Generates the body of the response to a GET method
 *
 * @param uri uri to the file
 * @return string with the body of the response
 */
std::string Location::getBody(std::string const& uri) const
{
	std::string path;
	struct stat file_info;

	// TODO: comprobar permisos de acceso
	// TODO: comprobar si es un directorio -> comprobar autoindex
	// TODO: comprobar si es un directorio -> comprobar index
	path = getPathTo(uri);
	if (access(path.c_str(), R_OK) < 0) {
		// TODO: que retorna?
	}

	if (stat(path.c_str(), &file_info) < 0) {
		// TODO: que retorna?
	}

	switch (file_info.st_mode) {
	case S_IFDIR: // directory file
		for (std::vector<std::string>::const_iterator it = this->_index.begin(); it != this->_index.end(); it++) {
			if (access(path.c_str(), R_OK) == 0) {
				// TODO: el archivo existe
				// TODO: deberia retornar (?)
				return readFile(path);
				// funcion para poner todo el archivo y retornarlo
			}
		}
		if (this->_autoindex)
			return autoIndex(path);
		// TODO: not found
		break;
	
	case S_IFREG: // regular file
		// funcion para poner todo el archivo y retornarlo
		return readFile(path);
		break;
	
	default:
		// TODO: error
		// Error en funcion de errno
		break;
	}
}

/**
 * Generates string with all the content in a given file
 *
 * @param path path to the file to read
 * @return string with all the content in the file
 */
std::string Location::readFile(std::string const& path) const
{
	std::fstream file;
	std::string  buffer, final;

	file.open(path);
	if (!file.is_open()) // TODO: error?
		return std::string("");
	
	while (getline(file, buffer)) {
		final += buffer;
		final.push_back('\n');
	}

	file.close();
	return final;
}

/**
 * Generates html with the files in a given directory
 *
 * @param path path to the directory
 * @return string with a list of files in the directory in html
 */
std::string Location::autoIndex(std::string const& path) const
{
	// TODO: y si termina en '/'?
	std::stringstream    buffer;
	DIR*                 directory;
	struct dirent*       file;
	struct stat          file_info;
	char                 date[512];

	directory = opendir(path.c_str());
	if (directory == NULL) {
		// TODO
	}

	buffer << "<html>\n"
	       << "<head><title>Index of " << path << "</title></head>\n"
	       << "<body>\n"
	       << "<h1>Index of " << path << "</h1><hr><pre><a href=\"../\">../</a>\n";

	while (true) {
		std::memset(&file_info, 0, sizeof(struct stat));
		std::memset(&date, 0, 512);
		
		file = readdir(directory);

		if (file == NULL) {
			// TODO: control de errores
			break;
		}

		if (file->d_name[0] == '.')
			continue;

		stat(file->d_name, &file_info);
		if (file == NULL)
			break;

		std::strftime(date, 512, "%d-%B-%Y %H:%M", std::gmtime(&file_info.st_mtim.tv_sec));

		buffer << "<a href=\"" << file->d_name << "\">"                        // NOTE: link to the file
		       << std::left << std::setw(80) << file->d_name << "</a>"         // NOTE: text of the link
		       << std::setw(1) << date << "\t" << file_info.st_size << "\n"; // NOTE: file info
	}

	buffer << "</pre><hr></body>\n"
	       << "</html>\n";

	closedir(directory);

	return buffer.str();
}
