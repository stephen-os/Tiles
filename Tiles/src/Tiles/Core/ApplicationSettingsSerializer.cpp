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
			settings.Window.Width = window.value("width", settings.Window.Width);
			settings.Window.Height = window.value("height", settings.Window.Height);
			settings.Window.PositionX = window.value("x", settings.Window.PositionX);
			settings.Window.PositionY = window.value("y", settings.Window.PositionY);
			settings.Window.Maximized = window.value("maximized", settings.Window.Maximized);
			settings.Window.Fullscreen = window.value("fullscreen", settings.Window.Fullscreen);

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
			window["width"] = settings.Window.Width;
			window["height"] = settings.Window.Height;
			window["x"] = settings.Window.PositionX;
			window["y"] = settings.Window.PositionY;
			window["maximized"] = settings.Window.Maximized;
			window["fullscreen"] = settings.Window.Fullscreen;

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
