#pragma once

#include "imgui.h"

namespace Tiles::UI
{
	// The visual weight of a button, mapped to theme colors by the widget itself so
	// a call site cannot smuggle in an off-theme color.
	enum class ButtonVariant
	{
		Default,
		Primary,
		Danger
	};

	// A themed button. A size of {0, 0} fits the label. Returns true on the frame
	// it is clicked.
	[[nodiscard]] bool Button(const char* label, ButtonVariant variant = ButtonVariant::Default, const ImVec2& size = ImVec2(0.0f, 0.0f));

	// A button that reflects on/off state: accent-filled when active, surface
	// otherwise. Returns true on click; the caller owns the toggle. For tool
	// selection (brush / eraser / fill / ...).
	[[nodiscard]] bool ToggleButton(const char* label, bool active, const ImVec2& size = ImVec2(0.0f, 0.0f));

	// A full-width, bold, left-aligned section title strip.
	void SectionHeader(const char* title);

	// --- Typography ---

	// A heading: the bold font, scaled up from the body size.
	void TextTitle(const char* text);

	// Bold body text (bold font if loaded, else the body font).
	void TextBold(const char* text);

	// Secondary / hint text in the muted color.
	void TextMuted(const char* text);

	// Body text centered within the available content width.
	void TextCentered(const char* text);

	// A muted "(?)" marker that shows `desc` as a tooltip on hover.
	void HelpMarker(const char* desc);

	// --- Property rows ---

	// Begins a two-column label/control table. Returns false if it could not open
	// (then do not call PropertyLabel / EndPropertyTable).
	[[nodiscard]] bool BeginPropertyTable(const char* id);

	// Starts a property row: draws `label` in the left column and positions a
	// full-width control in the right. Follow with exactly one ImGui control.
	void PropertyLabel(const char* label);

	// Ends a property table opened with BeginPropertyTable.
	void EndPropertyTable();
}
