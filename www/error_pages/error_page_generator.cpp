#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#define OUTPUT_EXTENSION ".html"
#define ERROR_CODE       "__ERROR_CODE__"
#define ERROR_TEXT       "__ERROR_TEXT__"

static void init(std::map<std::string, std::string>& error_list)
{
	error_list["100"] = "Continue";
	error_list["101"] = "Switching Protocols";
	error_list["200"] = "OK";
	error_list["201"] = "Created";
	error_list["202"] = "Accepted";
	error_list["203"] = "Non-Authoritative Information";
	error_list["204"] = "No Content";
	error_list["205"] = "Reset Content";
	error_list["206"] = "Partial Content";
	error_list["300"] = "Multiple Choices";
	error_list["301"] = "Moved Permanently";
	error_list["302"] = "Found";
	error_list["303"] = "See Other";
	error_list["304"] = "Not Modified";
	error_list["305"] = "Use Proxy";
	error_list["307"] = "Temporary Redirect";
	error_list["308"] = "Permanent Redirect";
	error_list["400"] = "Bad Request";
	error_list["401"] = "Unauthorized";
	error_list["402"] = "Payment Required";
	error_list["403"] = "Forbidden";
	error_list["404"] = "Not Found";
	error_list["405"] = "Method Not Allowed";
	error_list["406"] = "Not Acceptable";
	error_list["407"] = "Proxy Authentication Required";
	error_list["408"] = "Request Timeout";
	error_list["409"] = "Conflict";
	error_list["410"] = "Gone";
	error_list["411"] = "Length Required";
	error_list["412"] = "Precondition Failed";
	error_list["413"] = "Content Too Large";
	error_list["414"] = "URI Too Long";
	error_list["415"] = "Unsupported Media Type";
	error_list["416"] = "Range Not Satisfiable";
	error_list["417"] = "Expectation Failed";
	error_list["421"] = "Misdirected Request";
	error_list["422"] = "Unprocessable Content";
	error_list["426"] = "Upgrade Required";
	error_list["500"] = "Internal Server Error";
	error_list["501"] = "Not Implemented";
	error_list["502"] = "Bad Gateway";
	error_list["503"] = "Service Unavailable";
	error_list["504"] = "Gateway Timeout";
	error_list["505"] = "HTTP Version Not Supported";
}

static void replace_key(std::string& line, std::string const& key, std::string const& str)
{
	std::size_t pos;

	while ((pos = line.find(key)) != std::string::npos)
		line.replace(pos, key.length(), str);
}

int main(int argc, char const *argv[])
{
	std::string                                  line;
	std::map<std::string, std::string>::iterator it;
	std::map<std::string, std::string>           error_list;
	std::ofstream                                created_file;
	std::ifstream                                template_file;
	std::string const                            key1 = ERROR_CODE;
	std::string const                            key2 = ERROR_TEXT;
	std::string const                            file_extension = OUTPUT_EXTENSION;

	if (argc != 2) {
		std::cerr <<  argv[0] << ": usage: " << argv[0] << " [FILE]" << std::endl;
		return EXIT_FAILURE;
	}

	init(error_list);

	for (it = error_list.begin(); it != error_list.end(); it++) {
		template_file.open(argv[1], std::ios::in);
		if (!template_file.is_open()) {
			std::cerr <<  argv[0] << ": error with " << argv[1] << ": " << std::strerror(errno) << std::endl;
			return EXIT_FAILURE;
		}

		created_file.open((it->first + file_extension).c_str(), std::ios::binary);
		if (!created_file.is_open()) {
			std::cerr <<  argv[0] << ": error with " << it->second << file_extension << ": " << std::strerror(errno) << std::endl;
			return EXIT_FAILURE;
		}

		while (std::getline(template_file, line)) {
			replace_key(line, key1, it->first);
			replace_key(line, key2, it->second);
			created_file << line << "\n";
		}

		created_file.close();
		template_file.close();
	}

	return EXIT_SUCCESS;
}
