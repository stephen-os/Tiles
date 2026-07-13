#pragma once

#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include <spdlog/spdlog.h>

namespace Tiles
{
	// A process-wide logger that manages two channels: Engine and Editor. 
	class Logger
	{
	public:
		enum class Type : uint8_t { Engine = 0, Editor };
		enum class Level : uint8_t { Trace = 0, Info, Warn, Error, Fatal };

		// Returns the process-wide logger instance.
		static Logger& Get();

		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;
		Logger(Logger&&) = delete;
		Logger& operator=(Logger&&) = delete;

		// Creates the Engine and Editor loggers over one shared colored console sink.
		void Init();

		// Flushes both loggers, shuts spdlog down, and releases them.
		void Shutdown();

		// Formats the arguments and logs the result on the given channel.
		template<typename... Args>
		void PrintMessage(Type type, Level level, std::format_string<Args...> format, Args&&... args);

		// Logs a failed-assertion line ("prefix: detail") at Error level.
		template<typename... Args>
		void PrintAssertMessage(Type type, std::string_view prefix, std::format_string<Args...> message, Args&&... args);

		// Logs a failed-assertion prefix with no detail message at Error level.
		void PrintAssertMessage(Type type, std::string_view prefix);

	private:
		Logger() = default;
		~Logger() = default;

		// Logs an already-formatted string verbatim on the given channel.
		void PrintMessageInternal(Type type, Level level, std::string_view message);

		std::shared_ptr<spdlog::logger> m_EngineLogger;
		std::shared_ptr<spdlog::logger> m_EditorLogger;
	};

	// Formats the arguments and forwards the string to the channel.
	template<typename... Args>
	void Logger::PrintMessage(Type type, Level level, std::format_string<Args...> format, Args&&... args)
	{
		PrintMessageInternal(type, level, std::format(format, std::forward<Args>(args)...));
	}

	// Joins the prefix and formatted detail and logs them at Error level.
	template<typename... Args>
	void Logger::PrintAssertMessage(Type type, std::string_view prefix, std::format_string<Args...> message, Args&&... args)
	{
		std::string formatted = std::format(message, std::forward<Args>(args)...);
		PrintMessageInternal(type, Level::Error, std::format("{}: {}", prefix, formatted));
	}
}

#define TILES_LOGGER_INIT()     ::Tiles::Logger::Get().Init()
#define TILES_LOGGER_SHUTDOWN() ::Tiles::Logger::Get().Shutdown()

// Engine channel -- the Tiles runtime
#define TILES_ENGINE_TRACE(...) ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Engine, ::Tiles::Logger::Level::Trace, __VA_ARGS__)
#define TILES_ENGINE_INFO(...)  ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Engine, ::Tiles::Logger::Level::Info,  __VA_ARGS__)
#define TILES_ENGINE_WARN(...)  ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Engine, ::Tiles::Logger::Level::Warn,  __VA_ARGS__)
#define TILES_ENGINE_ERROR(...) ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Engine, ::Tiles::Logger::Level::Error, __VA_ARGS__)
#define TILES_ENGINE_FATAL(...) ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Engine, ::Tiles::Logger::Level::Fatal, __VA_ARGS__)

// Editor channel -- the client app
#define TILES_EDITOR_TRACE(...) ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Editor, ::Tiles::Logger::Level::Trace, __VA_ARGS__)
#define TILES_EDITOR_INFO(...)  ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Editor, ::Tiles::Logger::Level::Info,  __VA_ARGS__)
#define TILES_EDITOR_WARN(...)  ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Editor, ::Tiles::Logger::Level::Warn,  __VA_ARGS__)
#define TILES_EDITOR_ERROR(...) ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Editor, ::Tiles::Logger::Level::Error, __VA_ARGS__)
#define TILES_EDITOR_FATAL(...) ::Tiles::Logger::Get().PrintMessage(::Tiles::Logger::Type::Editor, ::Tiles::Logger::Level::Fatal, __VA_ARGS__)
