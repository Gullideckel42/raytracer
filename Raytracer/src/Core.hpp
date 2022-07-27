#pragma once


// Standard headers
#include <iostream>
#include <string>
#include <stdio.h>
#include <memory>
#include <vector>

// Glew
#define GLEW_STATIC
#include <GL/glew.h>


#define RT_ ::rt::
#define RT_START namespace rt {
#define RT_END }
#ifdef RT_DEBUG
#define CORE_ASSERT(x) if(!x) __debugbreak();
#else
#define CORE_ASSERT(x) x
#endif

typedef bool RT_Result;