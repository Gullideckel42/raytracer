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
#ifdef RT_DEBUG
#define CORE_ASSERT(x) if(!x) {rt_fatal("Core", "Core assertion failed [", #x, "] at ", __FILE__ , ":", __LINE__); __debugbreak();};
#define RT_ASSERT(x) if(!x) { rt_error("Core", "Assertion failed [", #x, "] at ", __FILE__, ":", __LINE__); }
#else
#define CORE_ASSERT(x) x
#define RT_ASSERT(x) x
#endif

typedef bool RT_Result;

#include "Log/RT_Log.hpp"