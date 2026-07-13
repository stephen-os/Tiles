#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Tiles
{
	namespace
	{
		// ANSI escape codes used to color the console output fields.
		namespace LogColors
		{
			constexpr const char* RESET    = "\033[0m";
			constexpr const char* WHITE    = "\033[37m";
			constexpr const char* GRAY     = "\033[90m";
			constexpr const char* GREEN    = "\033[32m";
			constexpr const char* YELLOW   = "\033[33m";
			constexpr const char* RED      = "\033[31m";
			constexpr const char* CYAN     = "\033[36m";
			constexpr const char* ORANGE   = "\033[38;5;208m";
			constexpr const char* BOLD_RED = "\033[1;31m";
		}

		// Maps a spdlog level to the color its text is rendered in.
		const char* LevelColor(spdlog::level::level_enum level)
		{
			switch (level)
			{
			case spdlog::level::trace:    return LogColors::CYAN;
			case spdlog::level::debug:    return LogColors::WHITE;
			case spdlog::level::info:     return LogColors::GREEN;
			case spdlog::level::warn:     return LogColors::YELLOW;
			case spdlog::level::err:      return LogColors::RED;
			case spdlog::level::critical: return LogColors::BOLD_RED;
			default:                      return LogColors::WHITE;
			}
		}

		// spdlog formatter rendering "[time] [logger] [level] message" with ANSI
		// color per field for the console sink.
		class LogFormatter : public spdlog::formatter
		{
		public:
			// Writes one colored, field-separated log line into dest.
			void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) override
			{
				const char* levelColor = LevelColor(msg.level);
				std::string loggerName(msg.logger_name.data(), msg.logger_name.size());

				auto time = std::chrono::system_clock::to_time_t(msg.time);
				auto tm = spdlog::details::os::localtime(time);

				fmt::format_to(std::back_inserter(dest),
					"{}[{}{:02d}:{:02d}:{:02d}{}] "         // [TIME] - brackets white, time gray
					"{}[{}{}{}] "                           // [LOGGER] - brackets white, logger orange
					"{}[{}{}{}] "                           // [LEVEL] - brackets white, level colored
					"{}{}{}\n",                             // MESSAGE - white text + newline
					LogColors::WHITE, LogColors::GRAY, tm.tm_hour, tm.tm_min, tm.tm_sec, LogColors::WHITE,
					LogColors::WHITE, LogColors::ORANGE, loggerName, LogColors::WHITE,
					LogColors::WHITE, levelColor, spdlog::level::to_string_view(msg.level).data(), LogColors::WHITE,
					LogColors::WHITE, fmt::to_string(msg.payload), LogColors::RESET);
			}

			// Returns an independent copy of the formatter, as spdlog requires.
			std::unique_ptr<spdlog::formatter> clone() const override
			{
				return std::make_unique<LogFormatter>();
			}
		};
	}

	// Static accessor for the process-wide logger.
	Logger& Logger::Get()
	{
		static Logger instance;
		return instance;
	}

	// Creates the Engine and Editor loggers over one shared colored console sink.
	void Logger::Init()
	{
		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		consoleSink->set_formatter(std::make_unique<LogFormatter>());

		m_EngineLogger = std::make_shared<spdlog::logger>("ENGINE", consoleSink);
		m_EngineLogger->set_level(spdlog::level::trace);
		m_EngineLogger->flush_on(spdlog::level::err);

		m_EditorLogger = std::make_shared<spdlog::logger>("EDITOR", consoleSink);
		m_EditorLogger->set_level(spdlog::level::trace);
		m_EditorLogger->flush_on(spdlog::level::err);
	}

	// Flushes both loggers, shuts spdlog down, and releases them.
	void Logger::Shutdown()
	{
		if (m_EngineLogger)
			m_EngineLogger->flush();
		if (m_EditorLogger)
			m_EditorLogger->flush();

		spdlog::shutdown();

		m_EngineLogger.reset();
		m_EditorLogger.reset();
	}

	// Routes an already-formatted message to the channel's logger at the level.
	void Logger::PrintMessageInternal(Type type, Level level, std::string_view message)
	{
		std::shared_ptr<spdlog::logger>& logger = (type == Type::Engine) ? m_EngineLogger : m_EditorLogger;
		if (!logger)
			return;

		// Log the already-formatted string literally: passing it as the sole
		// argument selects spdlog's single-value overload, so any '{}' in the
		// message is not reinterpreted as a format placeholder.
		switch (level)
		{
		case Level::Trace: logger->trace(message);    break;
		case Level::Info:  logger->info(message);     break;
		case Level::Warn:  logger->warn(message);     break;
		case Level::Error: logger->error(message);    break;
		case Level::Fatal: logger->critical(message); break;
		}
	}

	// Logs a failed-assertion prefix with no detail message at Error level.
	void Logger::PrintAssertMessage(Type type, std::string_view prefix)
	{
		PrintMessageInternal(type, Level::Error, prefix);
	}
}
