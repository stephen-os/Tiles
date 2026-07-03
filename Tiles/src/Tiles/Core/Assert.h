#pragma once

#include "Log.h"
#include <cstdlib>

#define TILES_STRINGIFY2(x) #x
#define TILES_STRINGIFY(x)  TILES_STRINGIFY2(x)

#ifdef TILES_DEBUG

// Logs the failed condition with a formatted message and aborts (debug builds
// only; compiles to a no-op in release).
#define TILES_ASSERT(condition, ...) \
	do { \
		if (!(condition)) { \
			::Tiles::Logger::Get().PrintAssertMessage(::Tiles::Logger::Type::Core, \
				"Assertion (" #condition ") failed at " __FILE__ ":" TILES_STRINGIFY(__LINE__), __VA_ARGS__); \
			std::abort(); \
		} \
	} while (0)

#else

// No-op in release builds
#define TILES_ASSERT(condition, ...) do { (void)sizeof(condition); } while (0)

#endif
