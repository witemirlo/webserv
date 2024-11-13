#include <cstdlib>
#include <iostream>

#include "Server.hpp"

int main(int argc, char* argv[])
{
	std::cout << "HHHHHHHHHHHH" << std::endl;
	Server test(std::string("listen = 443\nserver_name = mundo.com"));
	std::cout << "I listen in: " << test.getListen() << std::endl;
	std::cout << "My name is: " << test.getServerName() << std::endl;
	return EXIT_SUCCESS;
	if (!argc && !argv[0])
		return 1;
}
