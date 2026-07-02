#pragma once

#include <memory>
#include <string>
#include <chrono>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace Tiles
{
	namespace LogColors
	{
		constexpr const char* RESET = "\033[0m";
		constexpr const char* WHITE = "\033[37m";
		constexpr const char* GRAY = "\033[90m";
		constexpr const char* GREEN = "\033[32m";
		constexpr const char* YELLOW = "\033[33m";
		constexpr const char* RED = "\033[31m";
		constexpr const char* CYAN = "\033[36m";
		constexpr const char* ORANGE = "\033[38;5;208m";
		constexpr const char* BOLD_RED = "\033[1;31m";
	}

	/// spdlog formatter rendering "[time] [logger] [level] message" with ANSI
	/// color per field for the console sink.
	class LogFormatter : public spdlog::formatter
	{
	public:
		LogFormatter() = default;
		void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) override;
		std::unique_ptr<spdlog::formatter> clone() const override;

	private:
		std::string GetLevelColor(spdlog::level::level_enum level);
	};

	class Log
	{
	public:
		/// Creates the shared console logger. Idempotent; later calls are no-ops.
		/// @param name Logger name shown in each line.
		static void Init(std::string& name);
		static void Shutdown();
		static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }
		static void SetLogLevel(spdlog::level::level_enum level);
		/// Adds a rotating file sink alongside the console sink.
		static void EnableFileLogging(const std::string& filename);

		/// Wraps each "{}" / "{...}" placeholder in a format string with ANSI
		/// color codes so substituted arguments stand out. Used by the log macros.
		static std::string Format(const std::string& format);
	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
		static std::vector<spdlog::sink_ptr> s_Sinks;
		static bool s_Initialized;
	};
}

#ifdef TILES_DEBUG
	#define TILES_LOG_TRACE(format, ...)    ::Tiles::Log::GetLogger()->trace(fmt::runtime(::Tiles::Log::Format(format)), __VA_ARGS__)
	#define TILES_LOG_INFO(format, ...)     ::Tiles::Log::GetLogger()->info(fmt::runtime(::Tiles::Log::Format(format)), __VA_ARGS__)
	#define TILES_LOG_WARN(format, ...)     ::Tiles::Log::GetLogger()->warn(fmt::runtime(::Tiles::Log::Format(format)), __VA_ARGS__)
	#define TILES_LOG_ERROR(format, ...)    ::Tiles::Log::GetLogger()->error(fmt::runtime(::Tiles::Log::Format(format)), __VA_ARGS__)
	#define TILES_LOG_CRITICAL(format, ...) ::Tiles::Log::GetLogger()->critical(fmt::runtime(::Tiles::Log::Format(format)), __VA_ARGS__)
#else
	#define TILES_LOG_TRACE(...)    (void)0
	#define TILES_LOG_INFO(...)     (void)0
	#define TILES_LOG_WARN(...)     (void)0
	#define TILES_LOG_ERROR(...)    (void)0
	#define TILES_LOG_CRITICAL(...) (void)0
#endif
