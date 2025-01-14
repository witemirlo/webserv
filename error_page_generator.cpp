#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

std::map<int, std::string> error_list = {
	{100, "Continue"},
	{101, "Switching Protocols"},
	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{203, "Non-Authoritative Information"},
	{204, "No Content"},
	{205, "Reset Content"},
	{206, "Partial Content"},
	{300, "Multiple Choices"},
	{301, "Moved Permanently"},
	{302, "Found"},
	{303, "See Other"},
	{304, "Not Modified"},
	{305, "Use Proxy"},
	{307, "Temporary Redirect"},
	{308, "Permanent Redirect"},
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Timeout"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length Required"},
	{412, "Precondition Failed"},
	{413, "Content Too Large"},
	{414, "URI Too Long"},
	{415, "Unsupported Media Type"},
	{416, "Range Not Satisfiable"},
	{417, "Expectation Failed"},
	{421, "Misdirected Request"},
	{422, "Unprocessable Content"},
	{426, "Upgrade Required"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Timeout"},
	{505, "HTTP Version Not Supported"}
};

int main(int argc, char const *argv[])
{
	std::ofstream file;
	int           error_code;

	for (int i = 1; i < argc; i++) {
		file.open((std::string(argv[i]) + ".html").c_str(), std::ios::binary);

		if (!file.is_open()) {
			std::cerr << "Error: " << argv[i] << ": " << std::strerror(errno) << std::endl;
			return EXIT_FAILURE;
		}

		error_code = std::atoi(argv[i]);
		if (error_list.find(error_code) == error_list.end()) {
			std::cerr << "Error: " << argv[i] << ": not a valid error number" << std::endl;
			continue;
		}

		file << 
			"<!doctype html>" "\n"
			"<html lang=\"en\">" "\n"
			"<head>" "\n"
				"\t" "<meta charset=\"utf-8\">" "\n"
				"\t" "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" "\n"
				"\t" "<title>" << error_code << " " << error_list[error_code] << "</title>" "\n"
				"\t" "<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH\" crossorigin=\"anonymous\">" "\n"
			"</head>" "\n"
			"<body>" "\n"
				"\t" "<div class=\"container h-100 d-flex justify-content-center\">" "\n"
					"\t\t" "<h1>" << error_code << "</h1>" "\n"
				"\t" "</div>" "\n"
				"\t" "<div class=\"container h-100 d-flex justify-content-center\">" "\n"
					"\t\t" "<p>" << error_list[error_code] << "</p>" "\n"
				"\t" "</div>" "\n"
				"\t" "<div class=\"container h-100 d-flex justify-content-center\">" "\n"
					"\t\t" "<p>" << "webserv" << "</p>" "\n"
				"\t" "</div>" "\n"
			"</body>" "\n"
			"</html>"
			<< std::endl;

		file.close();
	}
	return EXIT_SUCCESS;
}
