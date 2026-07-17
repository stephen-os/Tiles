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

	// --- Icon toggle + vector fields ---

	// An icon toggle button (ImageButton) that shows an accent border when active.
	// Returns true on click. For tool palettes.
	[[nodiscard]] bool ImageToggleButton(const char* strId, ImTextureID texture, bool active, const ImVec2& size);

	// A colored-chip + drag-float per component, editing the N floats at `v` in
	// place; chips use the theme axis colors. Return true on any change.
	[[nodiscard]] bool DragVec2(const char* id, float* v, float speed, float minVal, float maxVal, const char* format = "%.3f", const char* xName = "X", const char* yName = "Y");
	[[nodiscard]] bool DragVec3(const char* id, float* v, float speed, float minVal, float maxVal, const char* format = "%.3f", const char* xName = "X", const char* yName = "Y", const char* zName = "Z");
	[[nodiscard]] bool DragVec4(const char* id, float* v, float speed, float minVal, float maxVal, const char* format = "%.3f", const char* xName = "X", const char* yName = "Y", const char* zName = "Z", const char* wName = "W");

	// A full-width RGBA color editor (alpha bar + preview). Returns true on change.
	[[nodiscard]] bool ColorField(const char* id, float* rgba);

	// --- Read-only value displays ---

	// A full-width, non-interactive value box.
	void ValueField(const char* id, const char* text);

	// A colored-chip + read-only value box per component; chips use the theme axis
	// colors. For read-only vecN displays.
	void ValueVec2(const char* id, const float* v, const char* format = "%.3f", const char* xName = "X", const char* yName = "Y");
	void ValueVec3(const char* id, const float* v, const char* format = "%.3f", const char* xName = "X", const char* yName = "Y", const char* zName = "Z");
	void ValueVec4(const char* id, const float* v, const char* format = "%.3f", const char* xName = "X", const char* yName = "Y", const char* zName = "Z", const char* wName = "W");

	// A read-only, full-width RGBA color swatch.
	void ColorSwatch(const char* id, const float* rgba);
}
