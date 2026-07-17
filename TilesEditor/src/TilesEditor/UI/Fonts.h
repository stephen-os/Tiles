#pragma once

#include "imgui.h"

namespace Tiles::UI
{
	// A text role. Body is the default UI font; Bold is used for headings and
	// emphasis.
	enum class FontRole
	{
		Body,
		Bold
	};

	// Font files + base size to load. Paths resolve against the working directory
	// (the sandbox runs from TilesEditor/, so "res/fonts/...").
	struct FontConfig
	{
		const char* BodyPath = "res/fonts/Inter-Regular.ttf";
		const char* BoldPath = "res/fonts/Inter-Bold.ttf";
		float BaseSize = 16.0f;
	};

	// Loads the body + bold fonts if their files exist, else falls back to ImGui's
	// built-in font. Call once after the ImGui context exists and before the first
	// frame. Safe when the files are missing.
	void InitFonts(const FontConfig& config = {});

	// The font for a role. May be null, which ImGui treats as "keep current font".
	[[nodiscard]] ImFont* GetFont(FontRole role);

	// The base pixel size fonts were requested at (for size-relative scaling).
	[[nodiscard]] float GetBaseSize();
}
