#pragma once


// Standard headers
#include <iostream>
#include <stdio.h>
#include <memory>
#include <vector>

#define RT_ ::rt::
#define RT_START namespace rt {
#define RT_END }
#ifdef RT_DEBUG
#define CORE_ASSERT(x) if(!x) __debugbreak();
#else
#define CORE_ASSERT(x) x
#endif
