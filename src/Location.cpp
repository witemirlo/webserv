#include "Location.hpp"
#include "delimiter.hpp"
#include "HTTP_status_code.hpp"
#include "ARequest.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdio>
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
	std::pair<std::string, std::string>("jpg", "image/jpeg"),
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

Location::Location(void) : Server() {}

Location::Location(const Location &other)
	: Server(dynamic_cast<Server const&>(other))
{
	*this = other;
}

Location::~Location() {}

bool is_balanced(std::string const & line, size_t str, size_t end)
{
	int opened = 0;

	for (size_t ind = str + 1; ind < end; ind++)
	{
		if (line[ind] == STX)
			opened++;
		else if (line[ind] == ETX)
		{
			if (!opened)
				return false;
			opened--;
		}
	}
	return opened == 0;
}

/**
 * @param o the Server object used as the default configuration of this new location
 * @param config a std::string with the specifics of the configuration of the location
 * @param my_path the uri path of the locations, used to calculate its deepness
 */
Location::Location(Server const& o, std::string const & config, std::string const & my_path)
	: Server(o)
{
	std::string buffer, line, key, value;
	std::size_t ind, endtxt, starttxt;

	ind = 0;
	buffer = config;
	while (true) {


		starttxt = buffer.find(STX, ind);
		if (starttxt == std::string::npos)
				break ; //TODO: revisar que no se coma el ultimo

		endtxt = ind;
		while (true) {
			endtxt = buffer.find(ETX, endtxt);
			if (endtxt < starttxt || endtxt == std::string::npos)
			{
				std::cerr << RED "Error: " NC "unbalanced expresion in " + my_path + " location" << std::endl;
				exit(EXIT_FAILURE);
			}
			if (is_balanced(buffer, starttxt, endtxt))
				break ;
			else
				endtxt++;
		}

		line = buffer.substr(ind, endtxt - ind);
		ind = endtxt + 1;
		line.erase(line.find(STX), 1);

		key = line.substr(0, line.find('='));
		value = line.substr(line.find('=') + 1, line.size());
		
		// if (value.size() > 1 && *value.rbegin() != '/')
		// 	value.push_back('/'); TODO: esto lo he quitado porque creo que esta mal
		/*
		TODO: if key en prohibidas
			std::cerr key solo se permite en server
		*/

		procRule(key, value);

	}
	setErrorPages("");
	if (my_path == "/")
		this->_deepness = 0;
	else
		this->_deepness = std::count(my_path.begin(),  my_path.end(), '/');
}

Location &Location::operator=(const Location &other)
{
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
 * @param index bool for check in the index list
 * @return string with the full path to the file, if the file is a directory and there is an index, it returns the path to the file
 */
std::string Location::getPathTo(std::string const& uri, bool index) const
{
	// TODO: los directorios deben terminar en  / o con otro caracter?
	std::vector<std::string>::const_iterator index_it;
	std::string                              path;
	struct stat                              file_info;

	if (*(this->_root.rbegin()) == '/')
		path = this->_root.substr(0, this->_root.size() - 1);
	else
		path = this->_root;
	
	std::cerr << __FILE__  << ":" << __LINE__ << ": path: " << path << std::endl;
	if (this->_redirect.find(uri) == this->_redirect.end()) {
		std::cerr << __FILE__  << ":" << __LINE__ << ": redirect NOT founded" << std::endl;
		path += uri;
	}
	else {
		std::cerr << __FILE__  << ":" << __LINE__ << ": redirect founded" << std::endl;
		path = this->_root + this->_redirect.find(uri)->second;
	}
	
	std::cerr << __FILE__  << ":" << __LINE__ << ": path: " << path << std::endl;

	if (path.find(this->_cgi_extension) != std::string::npos)
		path = path.substr(0, (path.find(this->_cgi_extension) + this->_cgi_extension.length()));

	if (stat(path.c_str(), &file_info) < 0) {
		return "";
	}
	
	if (index && S_ISDIR(file_info.st_mode)) {
		for (index_it = this->_index.begin(); index_it != this->_index.end(); index_it++) {
			errno = 0;
			if (access((path + *index_it).c_str(), F_OK) == 0) {
				path += *index_it;
				break;
			}
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
	std::string const path = getPathTo(uri, true);
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
	std::stringstream body, headers;
	std::string const file_extension = getFileType(path);
	std::ifstream     file(path.c_str(), std::ios::binary);

	if (!file.is_open()) {
		errno = ENOENT;
		return "";
	}

	body << file.rdbuf();
	file.close();

	headers << "Content-Length: " << body.str().size() << CRLF
	        << "Content-Type: " << Location::getContentType(path) << CRLF
	        << CRLF;

	return (headers.str() + body.str());
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
	
	host_path = getPathTo(uri, true);
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
		value = line.substr(line.find(':') + 1);
		value.erase(std::remove_if(value.begin(), value.end(), isspace), value.end());
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
 * @param path path to the file
 * @return string with the corresponding content type
 */
std::string Location::getContentType(std::string const& path) const
{
	std::string const extension = getFileType(path);

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

	buffer << "Date: " << date << CRLF
	       << "Server: webserv" CRLF;
	
	if (status_code == 301)
		buffer << "Location: " << uri << CRLF;
	// TODO: meter headers correspondientes segun el status code
	// TODO: a la hora de parsear pasar todo a mayusculas/minusculas
	if (headers.find("content-type") == headers.end())
	{
		if (status_code >= 300) // TODO: seguro?
			buffer << "Content-Type: " << "text/html" CRLF;
		else
			buffer << "Content-Type: " << getContentType(uri) << CRLF; // TODO: poner bien el tipo
	}

	if (status_code == METHOD_NOT_ALLOWED)
	{
		buffer << "Allow: ";
		for (size_t i = 0; i < _allow.size(); i++){
			if (_allow[i] != "POST" || getFileType(uri) == _cgi_extension)
			{
				buffer << _allow[i];
				if (i != _allow.size() - 1)
					buffer << ", ";
			}
		}
		buffer << CRLF;
	}

	if (headers.find("content-length") == headers.end())
		buffer << "Content-Length: " << (body.size() - 2) << CRLF;

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
 * @brief Returns the status line corresponding to errno
 * @return status line 
 */
std::string Location::getStatusLine(void) const
{
	return getStatusLine(getStatusCode());
}

std::string Location::getStatusLine(unsigned int code) const // TODO: faltan un huevo xd
{
	std::stringstream token;

	token << code;

	return ("HTTP/1.1 " + token.str() + " " + getHttpMessage(code) + CRLF);
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
	std::stringstream                          headers, buffer;

	it = this->_error_pages.find(status_code);
	if (it == this->_error_pages.end()) std::cerr << __FILE__ << ":" << __LINE__ << " | it for status code " << status_code << " was not found" << std::endl;
	file.open(it->second.c_str());
	if (!file.is_open()) {
		// errno = EIO;
		// buffer << "An error ocurred with "
		//        << it->second
		//        << "\n";

		buffer << "<html><body>"
		       << "<h1 style=\"text-align: center\">" << status_code << " " << getHttpMessage(status_code) << "</h1>"
		       << "<p style=\"text-align: center\">webserv</p>"
		       << "</body></html>\n";

		headers << "Content-Length: " << buffer.str().size() << CRLF
	                << "Content-Type: " << "text/html" << CRLF
	                << CRLF;

		return headers.str() + buffer.str();
	}

	buffer << file.rdbuf();
	file.close();

	headers << "Content-Length: " << buffer.str().size() << CRLF
		<< "Content-Type: " << "text/html" << CRLF
	        << CRLF;

	return headers.str() + buffer.str();
}

std::string Location::getHttpMessage(int code) const
{
	switch (code) {
		case 100: return ("Continue");
		case 101: return ("Switching Protocols");
		case 200: return ("OK");
		case 201: return ("Created");
		case 202: return ("Accepted");
		case 203: return ("Non-Authoritative Information");
		case 204: return ("No Content");
		case 205: return ("Reset Content");
		case 206: return ("Partial Content");
		case 300: return ("Multiple Choices");
		case 301: return ("Moved Permanently");
		case 302: return ("Found");
		case 303: return ("See Other");
		case 304: return ("Not Modified");
		case 305: return ("Use Proxy");
		case 307: return ("Temporary Redirect");
		case 308: return ("Permanent Redirect");
		case 400: return ("Bad Request");
		case 401: return ("Unauthorized");
		case 402: return ("Payment Required");
		case 403: return ("Forbidden");
		case 404: return ("Not Found");
		case 405: return ("Method Not Allowed");
		case 406: return ("Not Acceptable");
		case 407: return ("Proxy Authentication Required");
		case 408: return ("Request Timeout");
		case 409: return ("Conflict");
		case 410: return ("Gone");
		case 411: return ("Length Required");
		case 412: return ("Precondition Failed");
		case 413: return ("Content Too Large");
		case 414: return ("URI Too Long");
		case 415: return ("Unsupported Media Type");
		case 416: return ("Range Not Satisfiable");
		case 417: return ("Expectation Failed");
		case 421: return ("Misdirected Request");
		case 422: return ("Unprocessable Content");
		case 426: return ("Upgrade Required");
		case 500: return ("Internal Server Error");
		case 501: return ("Not Implemented");
		case 502: return ("Bad Gateway");
		case 503: return ("Service Unavailable");
		case 504: return ("Gateway Timeout");
		case 505: return ("HTTP Version Not Supported");
		default:  return ("");
	}
}

void Location::callPOSTcgi(std::string const& uri, std::string const& type, std::string const& len) const
{
	std::string type_var = "CONTENT_TYPE=" + type;
	std::string len_var = "CONTENT_LENGTH=" + len;
	std::string file_var = "SCRIPT_FILENAME=" + getPathTo(uri, false);
	std::string path_var = "PATH_INFO=" + uri;
	std::string method = "REQUEST_METHOD=POST";
	std::string redirect = "REDIRECT_STATUS=true";

	chdir(_root.c_str());

	// std::cerr << "My variables" << std::endl;
	// std::cerr << type_var << std::endl;
	// std::cerr << len_var << std::endl;
	// std::cerr << file_var << std::endl;
	// std::cerr << method << std::endl;
	// std::cerr << redirect << std::endl;

	// char buff[101];
	// buff[100] = 0;
	// read(0, buff, atoi(len.c_str()));
	// std::cerr << __FILE__ << ":" << __LINE__  << " |  Small peek: " << std::string(buff) << std::endl;

	int count;
	for (count = 0; environ[count]; count++) {}
	
	char const ** new_envp = new char const *[count + 8];
	char * argv[] = {(char *)"/usr/bin/php-cgi", NULL};

	for (count = 0; environ[count]; count++) {
		new_envp[count] = environ[count];
	}
	new_envp[count] = type_var.c_str(); 
	new_envp[count + 1] = file_var.c_str(); 
	new_envp[count + 2] = method.c_str(); 
	new_envp[count + 3] = redirect.c_str();
	new_envp[count + 4] = len_var.c_str();
	new_envp[count + 5] = path_var.c_str();
	new_envp[count + 6] = NULL; 


	execve("/usr/bin/php-cgi", argv, (char * const *)new_envp);
	delete [] new_envp;
	std::cerr << __FILE__ << ":" << __LINE__ << ": Server::callPOSTcgi()" << std::endl;
	exit(errno);
}

std::string Location::CGIpost(std::string const& uri, std::string const& body, std::string const& type, std::string const& len) const
{
	errno = 0;
	int readpipe[2];
	int writepipe[2];
	pipe(readpipe);
	pipe(writepipe);

	pid_t pid = fork(); //TODO: fallo?
	if (pid == 0)
	{
		dup2(readpipe[1], STDOUT_FILENO);
		dup2(writepipe[0], STDIN_FILENO);
		close(writepipe[0]);
		close(writepipe[1]);
		close(readpipe[0]);
		close(readpipe[1]);
		callPOSTcgi(uri, type, len);
	}
	close(writepipe[0]);
	write(writepipe[1], body.c_str(), body.size());
	close(writepipe[1]);
	close(readpipe[1]);

	return read_cgi_response(readpipe[0]); //TODO: y si algo del otro lado ha ido mal??
	// TODO: waitpid para sacar el exit status (errno, setear en global para luego)
}

/**
 * @brief generates the response of a get request
 * @param uri uri to the file
 * @return string with the content of the response
 */
std::string Location::responsePOST(std::string const& uri, std::string const& msg, std::string const& type, std::string const& len) const
{
	std::string body;

	if (std::find(_allow.begin(), _allow.end(), "POST") == _allow.end())
		return (responseGET(METHOD_NOT_ALLOWED, uri));

	if (getFileType(uri) == _cgi_extension)
	{
		body = CGIpost(uri, msg, type, len);
		return (getStatusLine(OK) + body);
	}
	return (responseGET(METHOD_NOT_ALLOWED, uri)); //TODO: ajustar con allowed methods
}

/**
 * @brief generates the response of a get request
 * @param uri uri to the file
 * @param query query of the request
 * @return string with the content of the response
 */
std::string Location::responseGET(std::string const& uri, std::string const& query) const
{
	if (std::find(_allow.begin(), _allow.end(), "GET") == _allow.end())
		return (responseGET(METHOD_NOT_ALLOWED, uri));

	std::string status_line, headers, body;
	int         status_code;
	(void)query;
	// std::cerr << __FILE__ << ":" << __LINE__  << " |  uri: " << uri << std::endl;
	// std::cerr << __FILE__ << ":" << __LINE__  << " |  query: " << query << std::endl;

	// TODO: leer lo que quiera que haya fallado al procesar la respuesta

	if (getFileType(uri) == _cgi_extension)// no siempre activa cgi, y la extension no necesariamente tiene la extencion .php
		body = CGIget(uri, query);// content lenght y content type
	else
	{
		body = getBody(uri);
		// body = CRLF + body;// TODO: que body plante un salto de linea
	}
	// std::cerr << __FILE__ << ":" << __LINE__  << " |  This is body: " << body << std::endl;
	status_code = getStatusCode();
			// std::cerr << __FILE__ << ":" << __LINE__ << " | status code: " << status_code << std::endl;
	if (status_code >= 300) {// TODO: no me acaba de convencer esto, la redireccion entraria aqui
    		// TODO: mirar las error pages
		body = getBodyError(status_code);
	}
	// TODO: comprobar el status code y trabajar en consecuencia

	headers = getHeaders(body, uri, status_code);
			// std::cerr << __FILE__ << ":" << __LINE__ << " | headers: " << headers << std::endl;
	// status_line = getStatusLine(status_code);
	// status_line = "HTTP/1.1 200 OK\r\n";// TODO: hardcode
	status_line = getStatusLine();
			// std::cerr << __FILE__ << ":" << __LINE__ << "| response:\n" << (status_line + headers + body) << std::endl;
			// std::cerr << __FILE__ << ":" << __LINE__ << " | response:\n";
			// std::cout << (status_line + headers + body);
			// std::cout << std::flush;
		// std::cerr << __FILE__ << ":" << __LINE__ << " | response:\n" << (status_line + headers + body) << std::endl;
	return (status_line + headers + body);
}


std::string Location::responseGET(unsigned int error_code) const
{
	std::string const body = getBodyError(error_code);

	return (getStatusLine(error_code) + getHeaders(body, "", error_code)  + body);
}

std::string Location::responseGET(unsigned int error_code, std::string const& uri) const
{
	std::string const body = getBodyError(error_code);

	return (getStatusLine(error_code) + getHeaders(body, uri, error_code)  + body);
}

std::string Location::responseDELETE(std::string const& uri, std::string const& query) const
{
	if (std::find(_allow.begin(), _allow.end(), "DELETE") == _allow.end())
		return (responseGET(METHOD_NOT_ALLOWED, uri));

	std::string file_path;
	struct stat file_info;
	// int         status_code;// TODO: que en todas las que se hace esto se llame a la funcion en su lugar

	(void)uri;
	(void)query;// TODO: esto haria falta para algo?

	errno = 0;
	std::memset(&file_info, 0, sizeof(struct stat));

	file_path = getPathTo(uri, false);

	// TODO: cuales son los permisos para borrar un archivo?
	// TODO: delete de un archivo que no existe?
	if (access(file_path.c_str(), F_OK) < 0) { // TODO: si existe pero no tiene permisos internal server error
		return (getStatusLine() + getHeaders(CRLF, uri, NOT_FOUND) + CRLF);
	}

	if (stat(file_path.c_str(), &file_info) < 0) {
		return (getStatusLine() + getHeaders(CRLF, uri, INTERNAL_SERVER_ERROR) + CRLF);
	}

	if (std::remove(file_path.c_str()) < 0) {
		return (getStatusLine() + getHeaders(CRLF, uri, INTERNAL_SERVER_ERROR) + CRLF);
	}

	return (getStatusLine(NO_CONTENT) + getHeaders(CRLF, uri, getStatusCode()) + CRLF);
}

std::string read_cgi_response(int fd)
{
	char buffer[BUFSIZ];
	std::string str;
	std::stringstream length;

	while (42) {
		memset(buffer, 0, BUFSIZ);
		if (read(fd, buffer, BUFSIZ - 1) <= 0)
			break ;
		str = str + std::string(buffer);
	}

	size_t crlf = str.find(CRLF CRLF);
	length << (str.size() - crlf - 4);
	std::string response;
	length >> response;
	response = "Content-Length: " + response + CRLF + str;
	close(fd);
	return (response);
}

void Location::callGETcgi(std::string const& uri, std::string const& query) const //TODO: ordenar funciones
{
	std::string query_var = "QUERY_STRING=" + query;
	std::string file_var = "SCRIPT_FILENAME=" + getPathTo(uri, true);
	std::string path_var = "PATH_INFO=" + uri;
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
	
	char const ** new_envp = new char const *[count + 7];
	char * argv[] = {(char *)"/usr/bin/php-cgi", NULL};

	for (count = 0; environ[count]; count++) {
		new_envp[count] = environ[count];
	}
	new_envp[count] = query_var.c_str(); 
	new_envp[count + 1] = file_var.c_str(); 
	new_envp[count + 2] = method.c_str(); 
	new_envp[count + 3] = redirect.c_str();
	new_envp[count + 4] = path_var.c_str();
	new_envp[count + 5] = NULL; 


	execve("/usr/bin/php-cgi", argv, (char * const *)new_envp);
	delete [] new_envp;
	std::cerr << __FILE__ << ":" << __LINE__ << ": Server::callGETcgi()" << std::endl;
	exit(errno);
}

std::string Location::CGIget(std::string const& uri, std::string const& query) const //TODO: path_info, a ver si podemos dar una vuelta al envp
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
		callGETcgi(uri, query);
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
	std::size_t dot_pos, dir_pos;

	dot_pos = file.find_last_of('.');
	if (dot_pos ==  std::string::npos)
		return "";
	
	dot_pos += 1;
	
	dir_pos = file.find('/', dot_pos);
	if (dir_pos == std::string::npos)
		dir_pos = file.length() - dot_pos;
	else
		dir_pos -= dot_pos;

	return (file.substr(dot_pos, dir_pos));
}

/**
 * @brief copies a file
 * @param origin name of the file to be copied
 * @param dest name of the copied file
 * @return true in success, false in failure
 */
bool Location::copy_file(std::string const& body, std::string const& dest) const
{
	std::ofstream cloned_file;

	cloned_file.open(dest.c_str(), std::ios::binary);
	if (!cloned_file.is_open()) {
		std::cerr << RED "Error: " NC << dest << ": " << strerror(errno) << std::endl;
		return false;
	}

	cloned_file << body;

	cloned_file.close();

	return true;
}
