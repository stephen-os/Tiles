#include "ApplicationSettingsSerializer.h"

#include <fstream>

#include "json.hpp"

#include "Log.h"

namespace Tiles
{
	void ApplicationSettingsSerializer::Load(const std::filesystem::path& path, ApplicationSettings& settings)
	{
		if (!std::filesystem::exists(path))
			return;

		try
		{
			std::ifstream file(path);
			nlohmann::json json;
			file >> json;

			const auto& window = json.at("window");
			settings.Width = window.value("width", settings.Width);
			settings.Height = window.value("height", settings.Height);
			settings.PositionX = window.value("x", settings.PositionX);
			settings.PositionY = window.value("y", settings.PositionY);
			settings.Maximized = window.value("maximized", settings.Maximized);
			settings.Fullscreen = window.value("fullscreen", settings.Fullscreen);

			TILES_ENGINE_INFO("ApplicationSettings: Loaded window state from '{}'", path.string());
		}
		catch (const std::exception& e)
		{
			TILES_ENGINE_WARN("ApplicationSettings: Failed to load '{}': {}; using defaults", path.string(), e.what());
		}
	}

	void ApplicationSettingsSerializer::Save(const std::filesystem::path& path, const ApplicationSettings& settings)
	{
		try
		{
			nlohmann::json window;
			window["width"] = settings.Width;
			window["height"] = settings.Height;
			window["x"] = settings.PositionX;
			window["y"] = settings.PositionY;
			window["maximized"] = settings.Maximized;
			window["fullscreen"] = settings.Fullscreen;

			nlohmann::json json;
			json["window"] = window;

			std::ofstream file(path);
			file << json.dump(4);
		}
		catch (const std::exception& e)
		{
			TILES_ENGINE_WARN("ApplicationSettings: Failed to save '{}': {}", path.string(), e.what());
		}
	}
}
