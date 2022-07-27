#include <iostream>


#include "Log/RT_Log.hpp"




/*
 * Entry Point
 * 
 * 
 */
int main()
{
	
	
	rt_info("Started Raytracer: ", true);
	rt_warn("Renderer Timeout: ", 323, "ms");
	rt_error("Failed rendering frame! id:", 912992);
	rt_fatal("Segmentation fault", 2323, "MB", false);

	std::cout << "Jo" << std::endl;
 	return 0;
}