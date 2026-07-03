#include "Log.h"

namespace Tiles
{
	std::unique_ptr<spdlog::formatter> LogFormatter::clone() const
	{
		return std::make_unique<LogFormatter>();
	}

	void LogFormatter::format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest)
	{
		std::string level_color = GetLevelColor(msg.level);
		std::string logger_name(msg.logger_name.data(), msg.logger_name.size());

		auto time_t = std::chrono::system_clock::to_time_t(msg.time);
		auto tm = spdlog::details::os::localtime(time_t);

		fmt::format_to(std::back_inserter(dest),
			"{}[{}{:02d}:{:02d}:{:02d}{}] "         // [TIME] - brackets white, time gray
			"{}[{}{}{}] "                           // [LOGGER] - brackets white, logger green
			"{}[{}{}{}] "                           // [LEVEL] - brackets white, level colored
			"{}{}{}\n",                             // MESSAGE - white text + newline
			LogColors::WHITE, LogColors::GRAY, tm.tm_hour, tm.tm_min, tm.tm_sec, LogColors::WHITE,
			LogColors::WHITE, LogColors::ORANGE, logger_name, LogColors::WHITE,
			LogColors::WHITE, level_color, spdlog::level::to_string_view(msg.level).data(), LogColors::WHITE,
			LogColors::WHITE, fmt::to_string(msg.payload), LogColors::RESET);
	}

	std::string LogFormatter::GetLevelColor(spdlog::level::level_enum level)
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

	Logger& Logger::Get()
	{
		static Logger instance;
		return instance;
	}

	void Logger::Init()
	{
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_formatter(std::make_unique<LogFormatter>());

		m_EngineLogger = std::make_shared<spdlog::logger>("ENGINE", console_sink);
		m_EngineLogger->set_level(spdlog::level::trace);
		m_EngineLogger->flush_on(spdlog::level::err);

		m_ClientLogger = std::make_shared<spdlog::logger>("CLIENT", console_sink);
		m_ClientLogger->set_level(spdlog::level::trace);
		m_ClientLogger->flush_on(spdlog::level::err);
	}

	void Logger::Shutdown()
	{
		if (m_EngineLogger)
			m_EngineLogger->flush();
		if (m_ClientLogger)
			m_ClientLogger->flush();

		spdlog::shutdown();

		m_EngineLogger.reset();
		m_ClientLogger.reset();
	}

	void Logger::PrintMessageInternal(Type type, Level level, std::string_view message)
	{
		std::shared_ptr<spdlog::logger>& logger = (type == Type::Core) ? m_EngineLogger : m_ClientLogger;
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

	void Logger::PrintMessageTagInternal(Type type, Level level, std::string_view tag, std::string_view message)
	{
		auto it = m_EnabledTags.find(std::string(tag));
		if (it != m_EnabledTags.end())
		{
			const TagDetails& details = it->second;
			if (!details.Enabled || level < details.LevelFilter)
				return;
		}

		PrintMessageInternal(type, level, std::format("[{}] {}", tag, message));
	}

	void Logger::PrintAssertMessage(Type type, std::string_view prefix)
	{
		PrintMessageInternal(type, Level::Error, prefix);
	}

	const char* Logger::LevelToString(Level level)
	{
		switch (level)
		{
		case Level::Trace: return "Trace";
		case Level::Info:  return "Info";
		case Level::Warn:  return "Warn";
		case Level::Error: return "Error";
		case Level::Fatal: return "Fatal";
		}
		return "Trace";
	}

	Logger::Level Logger::LevelFromString(std::string_view s)
	{
		if (s == "Trace") return Level::Trace;
		if (s == "Info")  return Level::Info;
		if (s == "Warn")  return Level::Warn;
		if (s == "Error") return Level::Error;
		if (s == "Fatal") return Level::Fatal;
		return Level::Trace;
	}
}
