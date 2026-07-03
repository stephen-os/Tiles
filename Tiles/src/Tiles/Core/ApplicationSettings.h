#pragma once

#include <cstdint>
#include <string>

namespace Tiles
{
	/// Window and startup configuration for the Application. The window-geometry
	/// fields (size, position, maximized, fullscreen) are persisted between runs
	/// by ApplicationSettingsSerializer; Name/Icon/Use2DRenderer are set in code.
	struct ApplicationSettings
	{
		std::string Name = "Tiles App";
		std::string Icon = "";

		uint32_t Width = 1600;
		uint32_t Height = 900;
		int32_t PositionX = 100;
		int32_t PositionY = 100;

		bool Use2DRenderer = false;

		bool Fullscreen = false;
		bool Maximized = false;
	};
}
