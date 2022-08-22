#pragma once


// Standard headers
#include <iostream>
#include <string>
#include <stdio.h>
#include <memory>
#include <vector>
#include <thread>
// Glew
#define GLEW_STATIC
#include <GL/glew.h>


#define RT_ ::rt::
#define RT_START namespace rt {
#define RT_END }
#define RT_PI 3.1415926535898
#define RT_E  2.7182818284590
#ifdef RT_DEBUG
#define CORE_ASSERT(x) if(!x) {rt_fatal("Core", "Core assertion failed [", #x, "] at ", __FILE__ , ":", __LINE__); __debugbreak();};
#define RT_ASSERT(x) if(!x) { rt_error("Core", "Assertion failed [", #x, "] at ", __FILE__, ":", __LINE__); __debugbreak();}
#else
#define CORE_ASSERT(x) if(!x) {rt_fatal("Core", "Core assertion failed [", #x, "] at ", __FILE__ , ":", __LINE__);}
#define RT_ASSERT(x) if(!x) { rt_error("Core", "Assertion failed [", #x, "] at ", __FILE__, ":", __LINE__);}
#endif

typedef bool RT_Result;

#include "Log/RT_Log.hpp"