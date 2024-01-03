#ifndef UTIL_H
#define UTIL_H

#include "SDL.h"
#include <iostream>
#include <cassert>
#include <string>
#include <format>

#ifdef __DEBUG__
#define CUSTOM_ASSERT(x, message) assert(x && message) 
#else
#define ASSERT(x)
#endif

#endif