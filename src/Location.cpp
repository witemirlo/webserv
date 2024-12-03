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

	if (my_path == "/")// jeje xd TODO: es del pobre nombre de location
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

std::string Location::getPathTo(std::string const& uri) const
{
	return (this->_root + uri);
}

std::string Location::getBody(std::string const& uri) const
{
	std::string path = getPathTo(uri); // TODO: este metodo deberia ser privado
	struct stat sb;
	// TODO: comprobar permisos de acceso
	// TODO: comprobar si es un directorio -> comprobar autoindex
	// TODO: comprobar si es un directorio -> comprobar index
	if (access(path.c_str(), F_OK | R_OK) < 0) { // esto comprueba tambien directorios?
		// TODO: que retorna?
	}

	stat(path.c_str(), &sb);
	switch (sb.st_mode) {
	case S_IFDIR: // directory file
		// TODO: index
		for (std::vector<std::string>::const_iterator it = this->_index.begin(); it != this->_index.end(); it++) {
			if (access(path.c_str(), F_OK | R_OK) == 0) {
				// TODO: el archivo existe
				// TODO: deberia retornar (?)
				return readFile(path);
				// funcion para poner todo el archivo y retornarlo
			}
		}
		// TODO: autoindex
		// funcion para generar el autoindex
		return autoIndex(path);
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

std::string Location::readFile(std::string const& path) const
{
	std::fstream file;
	std::string  buffer, final;

	file.open(path);
	if (!file.is_open()) // TODO: error?
		return std::string("");
	
	while (getline(file, buffer))
		final += buffer;

	return final;
}

std::string Location::autoIndex(std::string const& path) const
{
	// TODO: y si termina en '/'?
	struct dirent*       file;
	DIR*                 directory;
	struct stat          sb;
	std::stringstream    buffer;
	char                 date[512];

	directory = opendir(path.c_str());
	if (directory == NULL) {
		// TODO
	}

	buffer <<  "<html>\n"
	       << "<head><title>Index of " << path << "</title></head>\n"
	       << "<body>\n"
	       << "<h1>Index of " << path << "</h1><hr><pre><a href=\"../\">../</a>\n";

	while (true) {
		std::memset(&sb, 0, sizeof(struct stat));
		std::memset(&date, 0, 512);
		
		file = readdir(directory);

		if (file == NULL) {
			// TODO: control de errores
			break;
		}

		if (file->d_name[0] == '.')
			continue;

		stat(file->d_name, &sb);
		if (file == NULL)
			break;

		std::strftime(date, 512, "%d-%B-%Y %H:%M", std::gmtime(&sb.st_mtim.tv_sec));

		buffer << "<a href=\""
		       << file->d_name // NOTE: cambiar, name
		       << "\">"
		       << std::left << std::setw(80) << file->d_name // NOTE: cambiar, name
		       << "</a>"
		       << std::setw(1) << date // NOTE: cambiar, ultima modificacion
		       << "\t"
		       << sb.st_size // NOTE: cambiar, size
		       << "\n";
	}

	buffer << "</pre><hr></body>\n"
	       << "</html>\n";

	closedir(directory);

	return buffer.str();
}
