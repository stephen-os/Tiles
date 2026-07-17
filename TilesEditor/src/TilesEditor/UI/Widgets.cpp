#include "Widgets.h"

#include "Theme.h"
#include "Fonts.h"
#include "StyleScope.h"

#include <cfloat>

namespace Tiles::UI
{
	// The base/hovered/active fill colors a button draws with.
	struct ButtonColors
	{
		ImVec4 Base;
		ImVec4 Hovered;
		ImVec4 Active;
	};

	// Resolves a variant to its theme color triple.
	static ButtonColors ColorsFor(ButtonVariant variant)
	{
		const Theme& theme = GetTheme();
		switch (variant)
		{
		case ButtonVariant::Primary:
			return { theme.Accent, theme.AccentHovered, theme.AccentActive };
		case ButtonVariant::Danger:
			return { theme.Danger, theme.DangerHovered, theme.DangerActive };
		default:
			return { theme.Surface, theme.SurfaceHovered, theme.SurfaceActive };
		}
	}

	// A themed button; see header.
	bool Button(const char* label, ButtonVariant variant, const ImVec2& size)
	{
		const Theme& theme = GetTheme();
		const ButtonColors colors = ColorsFor(variant);

		StyleColorScope color(ImGuiCol_Button, colors.Base);
		color.Push(ImGuiCol_ButtonHovered, colors.Hovered)
			.Push(ImGuiCol_ButtonActive, colors.Active)
			.Push(ImGuiCol_Text, theme.Text);

		return ImGui::Button(label, size);
	}

	// A state-reflecting toggle button; see header.
	bool ToggleButton(const char* label, bool active, const ImVec2& size)
	{
		const Theme& theme = GetTheme();
		const ButtonColors colors = active
			? ButtonColors{ theme.Accent, theme.AccentHovered, theme.AccentActive }
			: ButtonColors{ theme.Surface, theme.SurfaceHovered, theme.SurfaceActive };

		StyleColorScope color(ImGuiCol_Button, colors.Base);
		color.Push(ImGuiCol_ButtonHovered, colors.Hovered)
			.Push(ImGuiCol_ButtonActive, colors.Active)
			.Push(ImGuiCol_Text, theme.Text);

		return ImGui::Button(label, size);
	}

	// A full-width, bold, left-aligned section title; see header.
	void SectionHeader(const char* title)
	{
		const Theme& theme = GetTheme();

		// Same color for all three states so it reads as a static strip.
		StyleColorScope color(ImGuiCol_Button, theme.Surface);
		color.Push(ImGuiCol_ButtonHovered, theme.Surface)
			.Push(ImGuiCol_ButtonActive, theme.Surface)
			.Push(ImGuiCol_Text, theme.Text);

		StyleVarScope vars(ImGuiStyleVar_FrameRounding, 0.0f);
		vars.Push(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

		ImGui::PushFont(GetFont(FontRole::Bold), 0.0f);
		ImGui::Button(title, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
		ImGui::PopFont();
	}

	// A heading; see header.
	void TextTitle(const char* text)
	{
		ImGui::PushFont(GetFont(FontRole::Bold), GetBaseSize() * 1.4f);
		ImGui::TextUnformatted(text);
		ImGui::PopFont();
	}

	// Bold body text; see header.
	void TextBold(const char* text)
	{
		ImGui::PushFont(GetFont(FontRole::Bold), 0.0f);
		ImGui::TextUnformatted(text);
		ImGui::PopFont();
	}

	// Muted text; see header.
	void TextMuted(const char* text)
	{
		StyleColorScope color(ImGuiCol_Text, GetTheme().TextMuted);
		ImGui::TextUnformatted(text);
	}

	// Centered text; see header.
	void TextCentered(const char* text)
	{
		const float avail = ImGui::GetContentRegionAvail().x;
		const float width = ImGui::CalcTextSize(text).x;
		if (width < avail)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail - width) * 0.5f);
		ImGui::TextUnformatted(text);
	}

	// Muted help marker with a hover tooltip; see header.
	void HelpMarker(const char* desc)
	{
		TextMuted("(?)");
		ImGui::SetItemTooltip("%s", desc);
	}

	// Begins a label/control property table; see header.
	bool BeginPropertyTable(const char* id)
	{
		if (!ImGui::BeginTable(id, 2))
			return false;

		ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, 90.0f);
		ImGui::TableSetupColumn("control", ImGuiTableColumnFlags_WidthStretch);
		return true;
	}

	// Starts a property row; see header.
	void PropertyLabel(const char* label)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	// Ends a property table; see header.
	void EndPropertyTable()
	{
		ImGui::EndTable();
	}

	// An icon toggle button; see header.
	bool ImageToggleButton(const char* strId, ImTextureID texture, bool active, const ImVec2& size)
	{
		const Theme& theme = GetTheme();

		StyleColorScope colors(ImGuiCol_Button, theme.Surface);
		colors.Push(ImGuiCol_ButtonHovered, theme.SurfaceHovered)
			.Push(ImGuiCol_ButtonActive, theme.SurfaceActive);

		StyleVarScope vars(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
		if (active)
		{
			vars.Push(ImGuiStyleVar_FrameBorderSize, 2.0f);
			colors.Push(ImGuiCol_Border, theme.Accent);
		}

		return ImGui::ImageButton(strId, texture, size);
	}

	// One colored chip + full-width drag float for a single vecN component.
	static bool DragComponent(const char* id, const char* axis, const ImVec4& axisColor, float* value, float speed, float minVal, float maxVal, const char* format)
	{
		const Theme& theme = GetTheme();

		ImGui::PushID(id);

		const float chip = ImGui::GetFrameHeight();
		{
			StyleColorScope color(ImGuiCol_Button, axisColor);
			color.Push(ImGuiCol_ButtonHovered, axisColor)
				.Push(ImGuiCol_ButtonActive, axisColor)
				.Push(ImGuiCol_Text, theme.Text);
			ImGui::Button(axis, ImVec2(chip, chip));
		}

		ImGui::SameLine(0.0f, 2.0f);
		ImGui::SetNextItemWidth(-FLT_MIN);
		const bool changed = ImGui::DragFloat("##value", value, speed, minVal, maxVal, format);

		ImGui::PopID();
		return changed;
	}

	// A 2-component drag; see header.
	bool DragVec2(const char* id, float* v, float speed, float minVal, float maxVal, const char* format, const char* xName, const char* yName)
	{
		const Theme& theme = GetTheme();
		bool changed = false;

		ImGui::PushID(id);
		StyleVarScope pad(ImGuiStyleVar_CellPadding, ImVec2(3.0f, 0.0f));
		if (ImGui::BeginTable("##vec2", 2, ImGuiTableFlags_SizingStretchSame))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); changed |= DragComponent("x", xName, theme.AxisX, v + 0, speed, minVal, maxVal, format);
			ImGui::TableNextColumn(); changed |= DragComponent("y", yName, theme.AxisY, v + 1, speed, minVal, maxVal, format);
			ImGui::EndTable();
		}
		ImGui::PopID();
		return changed;
	}

	// A 3-component drag; see header.
	bool DragVec3(const char* id, float* v, float speed, float minVal, float maxVal, const char* format, const char* xName, const char* yName, const char* zName)
	{
		const Theme& theme = GetTheme();
		bool changed = false;

		ImGui::PushID(id);
		StyleVarScope pad(ImGuiStyleVar_CellPadding, ImVec2(3.0f, 0.0f));
		if (ImGui::BeginTable("##vec3", 3, ImGuiTableFlags_SizingStretchSame))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); changed |= DragComponent("x", xName, theme.AxisX, v + 0, speed, minVal, maxVal, format);
			ImGui::TableNextColumn(); changed |= DragComponent("y", yName, theme.AxisY, v + 1, speed, minVal, maxVal, format);
			ImGui::TableNextColumn(); changed |= DragComponent("z", zName, theme.AxisZ, v + 2, speed, minVal, maxVal, format);
			ImGui::EndTable();
		}
		ImGui::PopID();
		return changed;
	}

	// A 4-component drag; see header.
	bool DragVec4(const char* id, float* v, float speed, float minVal, float maxVal, const char* format, const char* xName, const char* yName, const char* zName, const char* wName)
	{
		const Theme& theme = GetTheme();
		bool changed = false;

		ImGui::PushID(id);
		StyleVarScope pad(ImGuiStyleVar_CellPadding, ImVec2(3.0f, 0.0f));
		if (ImGui::BeginTable("##vec4", 4, ImGuiTableFlags_SizingStretchSame))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); changed |= DragComponent("x", xName, theme.AxisX, v + 0, speed, minVal, maxVal, format);
			ImGui::TableNextColumn(); changed |= DragComponent("y", yName, theme.AxisY, v + 1, speed, minVal, maxVal, format);
			ImGui::TableNextColumn(); changed |= DragComponent("z", zName, theme.AxisZ, v + 2, speed, minVal, maxVal, format);
			ImGui::TableNextColumn(); changed |= DragComponent("w", wName, theme.AxisW, v + 3, speed, minVal, maxVal, format);
			ImGui::EndTable();
		}
		ImGui::PopID();
		return changed;
	}

	// A full-width RGBA color editor; see header.
	bool ColorField(const char* id, float* rgba)
	{
		ImGui::PushID(id);
		ImGui::SetNextItemWidth(-FLT_MIN);
		const ImGuiColorEditFlags flags =
			ImGuiColorEditFlags_AlphaBar |
			ImGuiColorEditFlags_AlphaPreview |
			ImGuiColorEditFlags_DisplayRGB |
			ImGuiColorEditFlags_NoLabel;
		const bool changed = ImGui::ColorEdit4("##color", rgba, flags);
		ImGui::PopID();
		return changed;
	}
}
