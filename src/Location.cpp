#include "Location.hpp"
#include "delimiter.hpp"
#include "ARequest.hpp"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>

const std::string Location::prohibited_rules[] =  {"listen", "server_name", ""};
const std::pair<std::string, std::string> _file_types[] = {
	std::pair<std::string, std::string>("html", "text/html"),
	std::pair<std::string, std::string>("css",  "text/css"),
	std::pair<std::string, std::string>("js",   "text/javascript"),
	std::pair<std::string, std::string>("gif",  "image/gif"),
	std::pair<std::string, std::string>("png",  "image/png"),
	std::pair<std::string, std::string>("jpeg", "image/jpeg"),
	std::pair<std::string, std::string>("bmp",  "image/bmp"),
	std::pair<std::string, std::string>("webp", "image/webp"),
	std::pair<std::string, std::string>("mpeg", "audio/mpeg"),
	std::pair<std::string, std::string>("weba", "audio/webm"),
	std::pair<std::string, std::string>("oga",  "audio/ogg"),
	std::pair<std::string, std::string>("wav",  "audio/wav"),
	std::pair<std::string, std::string>("webm", "video/webm"),
	std::pair<std::string, std::string>("ogv",  "video/ogg"),
	std::pair<std::string, std::string>("pdf",  "application/pdf"),
	std::pair<std::string, std::string>("txt",  "text/plain"),
	std::pair<std::string, std::string>("",     "text/plain")
	// std::pair<std::string, std::string>("",     "application/octet-stream") // por defecto si tiene extension no conocida
};

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
	setErrorPages("");
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
 * @param status_code referencte to a int to set the status code
 * @return string with the body of the response and sets the status_code
 */
std::string Location::getBody(std::string const& uri) const
{
	std::vector<std::string>::const_iterator index_it;
	std::string                              path;
	struct stat                              file_info;

	errno = 0;
	path = getPathTo(uri);

	if (access(path.c_str(), R_OK) < 0)
		return "";

	if (stat(path.c_str(), &file_info) < 0)
		return "";

	switch (file_info.st_mode) {
	case S_IFDIR: // NOTE: directory file
		for (index_it = this->_index.begin(); index_it != this->_index.end(); index_it++) {
			if (access((path + *index_it).c_str(), R_OK) == 0)
				return readFile(path);
		}
		if (this->_autoindex) {
			return autoIndex(path);
		}
		break;
	
	case S_IFREG: // NOTE: regular file
		return readFile(path);
		break;
	
	default:
		break;
	}
	errno = ENOENT;
	return "";
}

/**
 * Generates string with all the content in a given file
 *
 * @param path path to the file to read
 * @return string with all the content in the file
 */
std::string Location::readFile(std::string const& path) const
{
	// TODO: esta funcion es la que tiene que que mirar si es un php
	std::fstream file(path.c_str());
	std::string  buffer, final;

	if (!file.is_open()) {
		errno = ENOENT;
		return std::string("");
	}
	
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
		return "";
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
			break;
		}

		if (file->d_name[0] == '.')
			continue;

		if (stat(file->d_name, &file_info) < 0) {
			closedir(directory);
			return "";
		}

		std::strftime(date, 512, "%d-%B-%Y %H:%M", std::gmtime(&file_info.st_mtim.tv_sec));

		buffer << "<a href=\"" << file->d_name << "\">"                      // NOTE: link to the file
		       << std::left << std::setw(80) << file->d_name << "</a>"       // NOTE: text of the link
		       << std::setw(1) << date << "\t" << file_info.st_size << "\n"; // NOTE: file info
	}

	buffer << "</pre><hr></body>\n"
	       << "</html>\n";

	closedir(directory);

	return buffer.str();
}

std::string Location::getHeaders(std::string const& body) const
{
	std::stringstream buffer;
	std::time_t       rawtime;
	struct tm*        gmt;
	char              date[512];

	time(&rawtime);
	gmt = gmtime(&rawtime);

	std::memset(&date, 0, 512);
	strftime(date, 512, "%a, %d %b %Y %H:%M:%S GMT", gmt);

	buffer << "date: " << date << "\r\n"
	       << "server: webserv\r\n"
	       << "content-type: text/html\r\n" // TODO: poner el que toca
	       << "content-lenght: " << body.size() << "\r\n"
	       << "\r\n";

	return buffer.str();
}

/**
 * Returns the status code corresponding to errno
 *
 * @return status code
 */
int Location::getStatusCode(void) const
{
	switch (errno) {
	case 0:
		return 200; // HINT: ok

	case ENAMETOOLONG:  // NOTE: pathname too long
		return 400; // HINT: bad request
	
	case ENOTDIR:       // NOTE: pathname contains a nondirectory as directory
		return 400; // HINT: bad request
	
	case EACCES:        // NOTE: the file exist but not have permissions
		return 403; // HINT: forbidden TODO: quizas un 404
	
	case ENOENT:        // NOTE: file not found
		return 404; // HINT: file not found
	
	default:
		return 500; // HINT: inernal server error
	}
}

/**
 * 
 *
 * @param 
 * @return 
 */
std::string Location::responseGET(std::string const& uri) const
{
	std::string status_line, headers, body;
	int         status_code;

	// TODO: leer lo que quiera que haya fallado al procesar la respuesta
	// TODO: comprobar la extension del archivo
	body = getBody(uri);
	status_code = getStatusCode();
	if (status_code != 200) {
    		// TODO: mirar las error pages
	}
	// TODO: comprobar el status code y trabajar en consecuencia
	headers = getHeaders(body);

	return (status_line + headers + body);
}

std::string read_cgi_response(int fd)
{
	char buffer[1024];
	std::string str;
	std::stringstream length;

	while (42) {
		memset(buffer, 0, 1024);
		if (read(fd, buffer, 1024 - 1) == 0)
			break ;
		str = str + std::string(buffer);
	}

	size_t crlf = str.find_last_of(CRLF);
	length << "Content-Length: " << str.size() - crlf - 2 << crlf;
	std::string response;
	length >> response;
	response = response + str;

	return (response);
}

std::string Location::CGIget(std::string const& file, std::string const& query) //TODO: path_info
{
	int pipefds[2];
	pipe(pipefds);

	pid_t pid = fork(); //TODO: fallo?
	if (pid == 0)
	{
		dup2(pipefds[1], STDOUT_FILENO);
		close(pipefds[0]);
		close(pipefds[1]);
		//TODO: callcgi
	}
	else
	{
		close(pipefds[0]);
		return read_cgi_response(pipefds[1]);
	}
}


std::string Location::getFileType(std::string const& file) const
{
	if (file.find('.') ==  std::string::npos)
		return "";

	return (file.substr(file.find_last_of('.') + 1));
}
