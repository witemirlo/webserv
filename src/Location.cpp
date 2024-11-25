#include "Location.hpp"

#include <iostream>

Location::Location(void)
{
	std::cout << GREEN "Location default constructor called" NC << std::endl;
}

Location::Location(const Location &other)
{
	std::cout << YELLOW "Location copy constructor called" NC << std::endl;
	*this = other;
}

Location &Location::operator=(const Location &other)
{
	std::cout << YELLOW "Location copy assignment operator called" NC << std::endl;
	return (*this);
}

Location::~Location()
{
	std::cout << RED "Location destrucutor called" NC << std::endl;
}
