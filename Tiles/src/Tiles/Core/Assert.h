#pragma once

#include "Log.h"
#include <cstdlib>

#ifdef TILES_DEBUG

#define TILES_ASSERT(condition, fmt, ...) \
	do { \
		if (!(condition)) { \
			::Tiles::Assert::LogFailure(#condition, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
			std::abort(); \
		} \
	} while (0)

#else

// No-op in release builds
#define TILES_ASSERT(condition, fmt, ...) do { (void)sizeof(condition); } while (0)

#endif

namespace Tiles::Assert
{
	template<typename... Args>
	inline void LogFailure(const char* condition, const char* file, int line, const char* fmt, Args&&... args)
	{
		TILES_LOG_ERROR("[ASSERT FAILED] Condition: {}\nMessage: {}\nFile: {}\nLine: {}",
			condition,
			fmt::format(fmt, std::forward<Args>(args)...),
			file,
			line
		);
	}
}
