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
}
