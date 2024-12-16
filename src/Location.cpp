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
#include <unistd.h>

const std::string Location::prohibited_rules[] =  {"listen", "server_name", ""};
const std::pair<std::string, std::string> Location::_file_types[] = {
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
	std::pair<std::string, std::string>("",     "application/octet-stream") // por defecto si tiene extension no conocida
};

extern char **environ;

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
 * @brief Generates the full path to a file
 * @param uri uri to the file
 * @return string with the full path to the file, if the file is a directory and there is an index, it returns the path to the file
 */
std::string Location::getPathTo(std::string const& uri) const
{
	// TODO: los directorios deben terminar en  / o con otro caracter?
	std::vector<std::string>::const_iterator index_it;
	std::string path;

	if (*(this->_root.rbegin()) == '/')
		path = this->_root.substr(0, this->_root.size() - 1) + uri;
	else
		path = this->_root + uri;

	// TODO: comprobar si es un directorio
	for (index_it = this->_index.begin(); index_it != this->_index.end(); index_it++) {
		errno = 0;
		if (access((path + *index_it).c_str(), R_OK) == 0) {
			path += *index_it;
			break;
		}
	}

	return path;
}

/**
 * @brief Generates the body of the response to a GET method
 * @param uri uri to the file
 * @param status_code referencte to a int to set the status code
 * @return string with the body of the response and sets the status_code
 */
std::string Location::getBody(std::string const& uri) const
{
	std::string const path = getPathTo(uri);
	struct stat file_info;

	errno = 0;
	std::memset(&file_info, 0, sizeof(struct stat));

	if (access(path.c_str(), R_OK) < 0)
		return "";

	if (stat(path.c_str(), &file_info) < 0)
		return "";

	if (S_ISREG(file_info.st_mode))
		return readFile(path);

	if (S_ISDIR(file_info.st_mode)) {
		if (this->_autoindex)
			return autoIndex(uri); // NOTE: uri instead path because it needs the original uri to make the correct relative links
	}

	errno = ENOENT;
	return "";
}

/**
 * @brief Generates string with all the content in a given file
 * @param path path to the file to read
 * @return string with all the content in the file
 */
std::string Location::readFile(std::string const& path) const
{
	// TODO: esta funcion es la que tiene que que mirar si es un php
	std::stringstream buffer;
	std::ifstream     file(path.c_str(), std::ios::binary);

	if (!file.is_open()) {
		errno = ENOENT;
		return "";
	}

	buffer << file.rdbuf();

	file.close();
	return (CRLF + buffer.str());
}

/**
 * @brief Generates html with the files in a given directory
 * @param uri uri to the directory
 * @return string with a list of files in the directory in html
 */
std::string Location::autoIndex(std::string const& uri) const
{
	std::stringstream body, headers;
	DIR*              directory;
	struct dirent*    file;
	struct stat       file_info;
	char              date[512];
	std::string       uri_path, host_path;
	
	host_path = getPathTo(uri);
	host_path = (*host_path.rbegin() == '/') ? host_path : (host_path + "/");
	uri_path  = (*uri.rbegin() == '/') ? uri : (uri + '/');

	directory = opendir(host_path.c_str());

	if (directory == NULL)
		return "";

	body << "<!DOCTYPE html>"
	     << "<html>\n"
	     << "<head><title>Index of " << uri_path << "</title></head>\n"
	     << "<body>\n"
	     << "<h1>Index of " << uri_path << "</h1><hr><pre><a href=\"" << (uri_path + "..") << "\">..</a>\n";

	while (true) {
		std::memset(&file_info, 0, sizeof(struct stat));
		std::memset(&date, 0, 512);
		
		file = readdir(directory);

		if (file == NULL)
			break;

		if (file->d_name[0] == '.')
			continue;

		if (stat((host_path + file->d_name).c_str(), &file_info) < 0) {
			closedir(directory);
			return "";
		}

		std::strftime(date, 512, "%d-%B-%Y %H:%M", std::gmtime(&file_info.st_mtim.tv_sec));

		body << "<a href=\"" << (uri_path + file->d_name) << "\">"           // NOTE: link to the file
		       << std::left << std::setw(80) << file->d_name << "</a>"       // NOTE: text of the link
		       << std::setw(1) << date << "\t" << file_info.st_size << "\n"; // NOTE: file info
	}

	body << "</pre><hr></body>\n"
	     << "</html>\n";

	closedir(directory);

	headers << "Content-Length: "  << body.str().size() << CRLF
	        << "Content-Type: text/html" CRLF
	        << CRLF;

	return (headers.str() + body.str());
}

/**
 * @brief gets all the headers generated by the cgi
 * @param body output of the cgi
 * @return map with the headers generated by the cgi
 */
std::map<std::string, std::string> Location::getCgiHeaders(std::string const& body) const
{
	std::map<std::string, std::string> headers;
	std::stringstream                  buffer(body);
	std::string                        line, key, value;
	
	while (getline(buffer, line)) {
		if (line == "\r")
			break;
		key = line.substr(0, line.find(':'));
		for (size_t i = 0; i < key.size(); i++)
			key[i] = tolower(key[i]);
		// std::cerr << __FILE__ << ": " << __LINE__  << " |  KEY: " << key << std::endl;
		value = line.substr(line.find(':') + 1);
		headers[key] = value;
		line.clear();
	}

	return headers;
}

/**
 * @brief generates the current time in GMt
 * @return string with the time
 */
std::string Location::getGmtTime(void) const
{
	std::time_t rawtime;
	struct tm*  gmt;
	char        date[512];

	time(&rawtime);
	gmt = gmtime(&rawtime);

	std::memset(&date, 0, 512);
	strftime(date, 512, "%a, %d %b %Y %H:%M:%S GMT", gmt);

	return std::string(date);
}

/**
 * @brief returns the content type for the response header
 * @param uri path to the file
 * @return string with the corresponding content type
 */
std::string Location::getContentType(std::string const& uri) const
{
	std::string const extension = getFileType(getPathTo(uri));

			// std::cerr << __FILE__ << ": " << __LINE__ << " | uri: " << "'" << uri << "'" << std::endl;
			// std::cerr << __FILE__ << ": " << __LINE__ << " | content type: " << "'" << extension << "'" << std::endl;
	if (extension == "")
		return "text/plain";

	for (int i = 0; Location::_file_types[i].first != ""; i++) {
		if (Location::_file_types[i].first == extension)
			return Location::_file_types[i].second;
	}
	
	return "application/octet-stream";
}

/**
 * @brief generates the corresponding headers for the response
 * @param body body of the response
 * @param uri uri of the request
 */
std::string Location::getHeaders(std::string const& body, std::string const& uri, int status_code) const
{
	std::map<std::string, std::string> headers;
	std::stringstream                  buffer;
	std::string                        date;

	headers = getCgiHeaders(body);
	date = getGmtTime();

	buffer << "date: " << date << CRLF
	       << "server: webserv" CRLF;
	
	// TODO: a la hora de parsear pasar todo a mayusculas/minusculas
	if (headers.find("content-type") == headers.end())
	{
		if (status_code != 200)
			buffer << "Content-Type: " << "text/html" CRLF;
		else
			buffer << "Content-Type: " << getContentType(uri) << CRLF; // TODO: poner bien el tipo
	}

	if (headers.find("content-length") == headers.end())
		buffer << "Content-Length: " << (body.size() - 2) << CRLF;

	// buffer << "\r\n";

	return buffer.str();
}

/**
 * @brief Returns the status code corresponding to errno
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
 * @brief generates the body of a error response
 * @param status_code status code of the response
 * @return body generated with the configured error page
 */
std::string Location::getBodyError(int status_code) const
{
	std::map<int, std::string>::const_iterator it;
	std::fstream                               file;
	std::string                                line;
	std::stringstream                          buffer;

	// std::cerr << "MY STATUS CODE IS " << status_code << std::endl;
	it = this->_error_pages.find(status_code);
	file.open(it->second.c_str());
	if (!file.is_open()) {
		buffer << "An error ocurred at opening "
		       << it->second
		       << "\n";
		return buffer.str();
	}

	buffer << CRLF;
	while (getline(file, line)) {
		buffer << line << '\n';
		line.clear();
	}

	file.close();
	return buffer.str();
}

/**
 * @brief generates the response of a get request
 * @param uri uri to the file
 * @param query query of the request
 * @return string with the content of the response
 */
std::string Location::responseGET(std::string const& uri, std::string const& query) const
{
	std::string status_line, headers, body;
	int         status_code;
	(void)query;
	// std::cerr << __FILE__ << ": " << __LINE__  << " |  uri: " << uri << std::endl;
	// std::cerr << __FILE__ << ": " << __LINE__  << " |  query: " << query << std::endl;

	// TODO: leer lo que quiera que haya fallado al procesar la respuesta

	// TODO: comprobar la extension del archivo
	if (getFileType(uri) == _cgi_extension)// no siempre activa cgi, y la extension no necesariamente tiene la extencion .php
		body = CGIget(getPathTo(uri), query);// content lenght y content type
	else
	{
		body = getBody(uri);
		// body = CRLF + body;// TODO: que body plante un salto de linea
	}
			// std::cerr << __FILE__ << ": " << __LINE__ << " | body:\n" << body << "EOF" << std::endl;
	// std::cerr << __FILE__ << ": " << __LINE__  << " |  This is body: " << body << std::endl;
	status_code = getStatusCode();
			// std::cerr << __FILE__ << ": " << __LINE__ << " | status code: " << status_code << std::endl;
	if (status_code != 200) {
    		// TODO: mirar las error pages
		body = getBodyError(status_code);
	}
	// TODO: comprobar el status code y trabajar en consecuencia

	headers = getHeaders(body, uri, status_code);
			// std::cerr << __FILE__ << ": " << __LINE__ << " | headers: " << headers << std::endl;
	// status_line = getStatusLine(status_code);
	status_line = "HTTP/1.1 200 OK\r\n";// TODO: hardcode
			// std::cerr << __FILE__ << ": " << __LINE__ << "| response:\n" << (status_line + headers + body) << std::endl;
			// std::cerr << __FILE__ << ": " << __LINE__ << " | response:\n";
			// std::cout << (status_line + headers + body);
			// std::cout << std::flush;
		std::cerr << __FILE__ << ": " << __LINE__ << "| response:\n" << (status_line + headers + body) << std::endl;
	return (status_line + headers + body);
}

std::string read_cgi_response(int fd)
{
	char buffer[1024];
	std::string str;
	std::stringstream length;

	while (42) {
		memset(buffer, 0, 1024);
		if (read(fd, buffer, 1024 - 1) <= 0)
			break ;
		str = str + std::string(buffer);
	}
	std::cerr << __FILE__ << ": " << __LINE__  << " |  CGI returns: " << str << std::endl;

	size_t crlf = str.find(CRLF CRLF);
	length << (str.size() - crlf - 4);
	std::string response;
	length >> response;
	std::cout << __FILE__ << ": " << __LINE__  << " |  My guarrada: " << response << std::endl;
	response = "Content-Length: " + response + CRLF + str;
	close(fd);
	return (response);
}

void Server::callcgi(std::string const& file, std::string const& query) const //TODO: ordenar funciones
{
	std::string query_var = "QUERY_STRING=" + query;
	std::string file_var = "SCRIPT_FILENAME=" + file;
	std::string method = "REQUEST_METHOD=GET";
	std::string redirect = "REDIRECT_STATUS=0";

	chdir(_root.c_str());

	// std::cerr << "My variables" << std::endl;
	// std::cerr << query_var << std::endl;
	// std::cerr << file_var << std::endl;
	// std::cerr << method << std::endl;
	// std::cerr << redirect << std::endl;

	int count;
	for (count = 0; environ[count]; count++) {}
	
	char const ** new_envp = new char const *[count + 5];
	char * argv[] = {(char *)"/usr/bin/php-cgi", NULL};

	for (count = 0; environ[count]; count++) {
		new_envp[count] = environ[count];
	}
	new_envp[count] = query_var.c_str(); 
	new_envp[count + 1] = file_var.c_str(); 
	new_envp[count + 2] = method.c_str(); 
	new_envp[count + 3] = redirect.c_str(); 
	new_envp[count + 4] = NULL; 


	execve("/usr/bin/php-cgi", argv, (char * const *)new_envp);
	delete [] new_envp;
	exit(errno);
}

std::string Location::CGIget(std::string const& file, std::string const& query) const //TODO: path_info, a ver si podemos dar una vuelta al envp
{
	errno = 0;
	int pipefds[2];
	pipe(pipefds);

	pid_t pid = fork(); //TODO: fallo?
	if (pid == 0)
	{
		dup2(pipefds[1], STDOUT_FILENO);
		close(pipefds[0]);
		close(pipefds[1]);
		callcgi(file, query);
	}
	close(pipefds[1]);
	return read_cgi_response(pipefds[0]); //TODO: y si algo del otro lado ha ido mal??
	// TODO: waitpid para sacar el exit status (errno, setear en global para luego)
}

/**
 * @brief gets the type of a file
 * @param file name/path of the file
 * @return extension (without the dot) of the file
 */
std::string Location::getFileType(std::string const& file) const
{
	if (file.find('.') ==  std::string::npos)
		return "";

	return (file.substr(file.find_last_of('.') + 1));
}
