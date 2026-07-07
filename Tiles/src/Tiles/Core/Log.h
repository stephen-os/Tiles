#pragma once

#include <cstdint>
#include <format>
#include <map>
#include <memory>
#include <string>
#include <string_view>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// TODO: Rename this file to Logger.h
// TODO: Add comments to each funciton, matching look of Application.h
// TODO: Simplify logging. We dont need this many macros. Lets keep what we need. 
namespace Tiles
{

	// TODO: move these to constants
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

	// spdlog formatter rendering "[time] [logger] [level] message" with ANSI
	// color per field for the console sink.
	class LogFormatter : public spdlog::formatter
	{
	public:
		LogFormatter() = default;
		void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) override;
		std::unique_ptr<spdlog::formatter> clone() const override;

	private:
		std::string GetLevelColor(spdlog::level::level_enum level);
	};

	// Tag-aware, std::format-based logger with distinct Core (ENGINE) and
	// Client (CLIENT) channels sharing a single colored console sink.
	class Logger
	{
	public:
		enum class Type : uint8_t { Core = 0, Client };
		enum class Level : uint8_t { Trace = 0, Info, Warn, Error, Fatal };

		struct TagDetails
		{
			bool Enabled = true;
			Level LevelFilter = Level::Trace;
		};

		static Logger& Get();

		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;
		Logger(Logger&&) = delete;
		Logger& operator=(Logger&&) = delete;

		// Creates the two console loggers sharing one colored stdout sink.
		void Init();
		// Flushes both loggers, shuts spdlog down, and releases the loggers.
		void Shutdown();

		std::shared_ptr<spdlog::logger>& GetEngineLogger() { return m_EngineLogger; }
		std::shared_ptr<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }

		bool HasTag(const std::string& tag) const { return m_EnabledTags.find(tag) != m_EnabledTags.end(); }
		std::map<std::string, TagDetails>& EnabledTags() { return m_EnabledTags; }

		template<typename... Args>
		void PrintMessage(Type type, Level level, std::format_string<Args...> format, Args&&... args);
		void PrintMessageInternal(Type type, Level level, std::string_view message);

		template<typename... Args>
		void PrintMessageTag(Type type, Level level, std::string_view tag, std::format_string<Args...> format, Args&&... args);
		void PrintMessageTagInternal(Type type, Level level, std::string_view tag, std::string_view message);

		template<typename... Args>
		void PrintAssertMessage(Type type, std::string_view prefix, std::format_string<Args...> message, Args&&... args);
		void PrintAssertMessage(Type type, std::string_view prefix);

		static const char* LevelToString(Level level);
		static Level LevelFromString(std::string_view s);

	private:
		Logger() = default;
		~Logger() = default;

		std::shared_ptr<spdlog::logger> m_EngineLogger;
		std::shared_ptr<spdlog::logger> m_ClientLogger;
		std::map<std::string, TagDetails> m_EnabledTags;
	};

	template<typename... Args>
	void Logger::PrintMessage(Type type, Level level, std::format_string<Args...> format, Args&&... args)
	{
		PrintMessageInternal(type, level, std::format(format, std::forward<Args>(args)...));
	}

	template<typename... Args>
	void Logger::PrintMessageTag(Type type, Level level, std::string_view tag, std::format_string<Args...> format, Args&&... args)
	{
		PrintMessageTagInternal(type, level, tag, std::format(format, std::forward<Args>(args)...));
	}

	template<typename... Args>
	void Logger::PrintAssertMessage(Type type, std::string_view prefix, std::format_string<Args...> message, Args&&... args)
	{
		std::string formatted = std::format(message, std::forward<Args>(args)...);
		PrintMessageInternal(type, Level::Error, std::format("{}: {}", prefix, formatted));
	}
}

#define TILES_LOGGER_INIT()     ::Tiles::Logger::Get().Init()
#define TILES_LOGGER_SHUTDOWN() ::Tiles::Logger::Get().Shutdown()

// Engine (Core) channel
#define TILES_ENGINE_TRACE(...) ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Core, ::Tiles::Logger::Level::Trace, __VA_ARGS__)
#define TILES_ENGINE_INFO(...)  ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Core, ::Tiles::Logger::Level::Info,  __VA_ARGS__)
#define TILES_ENGINE_WARN(...)  ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Core, ::Tiles::Logger::Level::Warn,  __VA_ARGS__)
#define TILES_ENGINE_ERROR(...) ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Core, ::Tiles::Logger::Level::Error, __VA_ARGS__)
#define TILES_ENGINE_FATAL(...) ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Core, ::Tiles::Logger::Level::Fatal, __VA_ARGS__)

#define TILES_ENGINE_TRACE_TAG(tag, ...) ::Tiles::Logger::Get().PrintMessageTag(::Tiles::Logger::Type::Core, ::Tiles::Logger::Level::Trace, tag, __VA_ARGS__)
#define TILES_ENGINE_INFO_TAG(tag, ...)  ::Tiles::Logger::Get().PrintMessageTag(::Tiles::Logger::Type::Core, ::Tiles::Logger::Level::Info,  tag, __VA_ARGS__)
#define TILES_ENGINE_WARN_TAG(tag, ...)  ::Tiles::Logger::Get().PrintMessageTag(::Tiles::Logger::Type::Core, ::Tiles::Logger::Level::Warn,  tag, __VA_ARGS__)
#define TILES_ENGINE_ERROR_TAG(tag, ...) ::Tiles::Logger::Get().PrintMessageTag(::Tiles::Logger::Type::Core, ::Tiles::Logger::Level::Error, tag, __VA_ARGS__)
#define TILES_ENGINE_FATAL_TAG(tag, ...) ::Tiles::Logger::Get().PrintMessageTag(::Tiles::Logger::Type::Core, ::Tiles::Logger::Level::Fatal, tag, __VA_ARGS__)

// Client channel
#define TILES_TRACE(...) ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Client, ::Tiles::Logger::Level::Trace, __VA_ARGS__)
#define TILES_INFO(...)  ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Client, ::Tiles::Logger::Level::Info,  __VA_ARGS__)
#define TILES_WARN(...)  ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Client, ::Tiles::Logger::Level::Warn,  __VA_ARGS__)
#define TILES_ERROR(...) ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Client, ::Tiles::Logger::Level::Error, __VA_ARGS__)
#define TILES_FATAL(...) ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Client, ::Tiles::Logger::Level::Fatal, __VA_ARGS__)

#define TILES_TRACE_TAG(tag, ...) ::Tiles::Logger::Get().PrintMessageTag(::Tiles::Logger::Type::Client, ::Tiles::Logger::Level::Trace, tag, __VA_ARGS__)
#define TILES_INFO_TAG(tag, ...)  ::Tiles::Logger::Get().PrintMessageTag(::Tiles::Logger::Type::Client, ::Tiles::Logger::Level::Info,  tag, __VA_ARGS__)
#define TILES_WARN_TAG(tag, ...)  ::Tiles::Logger::Get().PrintMessageTag(::Tiles::Logger::Type::Client, ::Tiles::Logger::Level::Warn,  tag, __VA_ARGS__)
#define TILES_ERROR_TAG(tag, ...) ::Tiles::Logger::Get().PrintMessageTag(::Tiles::Logger::Type::Client, ::Tiles::Logger::Level::Error, tag, __VA_ARGS__)
#define TILES_FATAL_TAG(tag, ...) ::Tiles::Logger::Get().PrintMessageTag(::Tiles::Logger::Type::Client, ::Tiles::Logger::Level::Fatal, tag, __VA_ARGS__)
