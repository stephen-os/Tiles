#pragma once

#include "Window.h"

#include <string>

namespace Tiles
{
	// Startup configuration for the Application: the full window spec plus the
	// app-level options. The Window's geometry fields (size, position, maximized,
	// fullscreen) are persisted between runs by ApplicationSettingsSerializer;
	// everything else is set in code.
	struct ApplicationSettings
	{
		WindowSettings Window;

		bool Use2DRenderer = false;

		// Working-directory-relative path of the persisted window-state file.
		std::string SettingsFile = "settings.json";
	};
}
