#pragma once

#include "imgui.h"

namespace Tiles::UI
{
	// The single source of truth for the app's visual tokens: the colors and
	// metrics every shared widget and the ImGui base style read from. Fill one
	// Theme, install it with Apply; widgets pull from GetTheme(), so the whole look
	// retunes in one place instead of the literals scattered across panels today.
	//
	// The defaults blend the Tiles palette (orange accent, dark grays) with an
	// After Effects-style structure: flat 2px rounding, compact spacing, and inset
	// 1px frame borders.
	struct Theme
	{
		// Window and surfaces (the neutral grays)
		ImVec4 WindowBg       { 0.10f, 0.10f, 0.10f, 1.00f };
		ImVec4 Surface        { 0.20f, 0.20f, 0.20f, 1.00f };
		ImVec4 SurfaceHovered { 0.30f, 0.30f, 0.30f, 1.00f };
		ImVec4 SurfaceActive  { 0.40f, 0.40f, 0.40f, 1.00f };

		// Accent (orange) -- headers, tabs, active states
		ImVec4 Accent         { 0.80f, 0.40f, 0.10f, 1.00f };
		ImVec4 AccentHovered  { 0.90f, 0.50f, 0.20f, 1.00f };
		ImVec4 AccentActive   { 1.00f, 0.60f, 0.30f, 1.00f };

		// Bright accent -- checkmarks, slider grabs
		ImVec4 AccentBright       { 1.00f, 0.50f, 0.00f, 1.00f };
		ImVec4 AccentBrightActive { 1.00f, 0.60f, 0.20f, 1.00f };

		// Danger -- destructive / reset actions
		ImVec4 Danger         { 0.80f, 0.30f, 0.30f, 1.00f };
		ImVec4 DangerHovered  { 0.90f, 0.40f, 0.40f, 1.00f };
		ImVec4 DangerActive   { 0.70f, 0.20f, 0.20f, 1.00f };

		// vecN component chips (X / Y / Z / W)
		ImVec4 AxisX          { 0.80f, 0.20f, 0.20f, 1.00f };
		ImVec4 AxisY          { 0.20f, 0.80f, 0.20f, 1.00f };
		ImVec4 AxisZ          { 0.20f, 0.20f, 0.80f, 1.00f };
		ImVec4 AxisW          { 0.40f, 0.40f, 0.40f, 1.00f };

		// Lines -- a dark border reads as an inset/sunken frame edge
		ImVec4 Border         { 0.05f, 0.05f, 0.06f, 1.00f };
		ImVec4 Separator      { 0.28f, 0.28f, 0.28f, 1.00f };

		// Text
		ImVec4 Text           { 1.00f, 1.00f, 1.00f, 1.00f };
		ImVec4 TextMuted      { 0.65f, 0.65f, 0.65f, 1.00f };

		// Metrics -- After Effects-style flat/compact structure over the Tiles palette
		float  WindowRounding = 2.0f;
		float  FrameRounding  = 2.0f;
		ImVec2 WindowPadding  { 10.0f, 10.0f };
		ImVec2 FramePadding   { 8.0f, 5.0f };
		ImVec2 ItemSpacing    { 6.0f, 4.0f };
		float  ScrollbarSize  = 11.0f;
		float  WindowBorderSize = 1.0f;
		float  FrameBorderSize  = 1.0f;
	};

	// Installs a theme: stores it for the widgets (GetTheme) and writes it onto the
	// ImGui base style -- colors, rounding, borders, and spacing. Requires an active
	// ImGui context. Safe to call every frame the theme changes.
	void Apply(const Theme& theme);

	// The theme currently in effect. The reference is valid until the next Apply.
	[[nodiscard]] const Theme& GetTheme();
}
