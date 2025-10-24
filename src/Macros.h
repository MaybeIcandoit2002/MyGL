#pragma once
#include <iostream>
#include "tests/TestMenu.h"
#if defined(_DEBUG)
	#define DEBUG_PRINT(x) std::cout << "DEBUG(" << __FUNCTION__ << "):\t" << x << std::endl
	#define DEBUG_RUN(x) x
#endif