#include <iostream>


#include "Log/RT_Log.hpp"




/*
 * Entry Point
 * 
 * 
 */
int main()
{
	
	rt::log::log(rt::log::LogLevel::ERROR, true, "asd", 2);
	rt::log::log(rt::log::LogLevel::INFO, true, "OpenGL Version: ", 23.2, "->", false);
	std::cout << "Jo" << std::endl;
 	return 0;
}