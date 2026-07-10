#pragma once

#include "Window.h"

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
	};
}
