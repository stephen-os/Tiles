#pragma once

#include <filesystem>

#include "ApplicationSettings.h"

namespace Tiles
{
	/// Persists the window-geometry fields of ApplicationSettings to a JSON file
	/// so window size, position, and maximized/fullscreen state survive restarts.
	/// Loading is best-effort: a missing or malformed file leaves the defaults.
	class ApplicationSettingsSerializer
	{
	public:
		/// Applies the persisted window state in @p path onto @p settings. A
		/// missing or unreadable file leaves @p settings unchanged.
		static void Load(const std::filesystem::path& path, ApplicationSettings& settings);
		/// Writes the window-geometry fields of @p settings to @p path as JSON.
		static void Save(const std::filesystem::path& path, const ApplicationSettings& settings);
	};
}
