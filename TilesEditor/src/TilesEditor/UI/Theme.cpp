#include "Theme.h"

namespace Tiles::UI
{
	// The one installed theme; starts at the editor-matching defaults from Theme.h.
	static Theme s_Theme;

	// Installs a theme onto both the widget layer and the full ImGui base style.
	void Apply(const Theme& theme)
	{
		s_Theme = theme;

		// Start from ImGui's dark preset, then override to the Tiles look so any
		// color we do not set still has a sensible dark value.
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();

		// One rounding value across every element reads as intentional.
		style.WindowRounding    = theme.WindowRounding;
		style.ChildRounding     = theme.FrameRounding;
		style.FrameRounding     = theme.FrameRounding;
		style.PopupRounding     = theme.FrameRounding;
		style.ScrollbarRounding = theme.FrameRounding;
		style.GrabRounding      = theme.FrameRounding;
		style.TabRounding       = theme.FrameRounding;

		style.WindowPadding     = theme.WindowPadding;
		style.FramePadding      = theme.FramePadding;
		style.ItemSpacing       = theme.ItemSpacing;
		style.ScrollbarSize     = theme.ScrollbarSize;
		style.GrabMinSize       = 10.0f;
		style.WindowTitleAlign  = ImVec2(0.0f, 0.5f);

		// Borders are theme-driven so a flat or an inset-frame look is a token away.
		style.WindowBorderSize  = theme.WindowBorderSize;
		style.ChildBorderSize   = theme.WindowBorderSize;
		style.PopupBorderSize   = theme.WindowBorderSize;
		style.FrameBorderSize   = theme.FrameBorderSize;

		const ImVec4 headerBase  = ImVec4(theme.Accent.x, theme.Accent.y, theme.Accent.z, 0.80f);
		const ImVec4 headerHover = ImVec4(theme.AccentHovered.x, theme.AccentHovered.y, theme.AccentHovered.z, 0.80f);

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_WindowBg]             = theme.WindowBg;
		colors[ImGuiCol_ChildBg]              = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_PopupBg]              = theme.WindowBg;
		colors[ImGuiCol_Border]               = theme.Border;
		colors[ImGuiCol_Text]                 = theme.Text;
		colors[ImGuiCol_TextDisabled]         = theme.TextMuted;
		colors[ImGuiCol_TitleBg]              = theme.Surface;
		colors[ImGuiCol_TitleBgActive]        = theme.SurfaceHovered;
		colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(theme.Surface.x, theme.Surface.y, theme.Surface.z, 0.60f);
		colors[ImGuiCol_MenuBarBg]            = theme.Surface;
		colors[ImGuiCol_FrameBg]              = theme.Surface;
		colors[ImGuiCol_FrameBgHovered]       = theme.SurfaceHovered;
		colors[ImGuiCol_FrameBgActive]        = theme.SurfaceActive;
		colors[ImGuiCol_Button]               = theme.Surface;
		colors[ImGuiCol_ButtonHovered]        = theme.SurfaceHovered;
		colors[ImGuiCol_ButtonActive]         = theme.SurfaceActive;
		colors[ImGuiCol_Header]               = headerBase;
		colors[ImGuiCol_HeaderHovered]        = headerHover;
		colors[ImGuiCol_HeaderActive]         = theme.AccentActive;
		colors[ImGuiCol_Separator]            = theme.Separator;
		colors[ImGuiCol_SeparatorHovered]     = theme.AccentHovered;
		colors[ImGuiCol_SeparatorActive]      = theme.AccentActive;
		colors[ImGuiCol_CheckMark]            = theme.AccentBright;
		colors[ImGuiCol_SliderGrab]           = theme.AccentBright;
		colors[ImGuiCol_SliderGrabActive]     = theme.AccentBrightActive;
		colors[ImGuiCol_ScrollbarBg]          = ImVec4(theme.WindowBg.x, theme.WindowBg.y, theme.WindowBg.z, 0.60f);
		colors[ImGuiCol_ScrollbarGrab]        = theme.Surface;
		colors[ImGuiCol_ScrollbarGrabHovered] = theme.SurfaceHovered;
		colors[ImGuiCol_ScrollbarGrabActive]  = theme.SurfaceActive;
		colors[ImGuiCol_ResizeGrip]           = ImVec4(theme.Accent.x, theme.Accent.y, theme.Accent.z, 0.30f);
		colors[ImGuiCol_ResizeGripHovered]    = headerHover;
		colors[ImGuiCol_ResizeGripActive]     = theme.AccentActive;

		// Tabs: inactive gray, active accent (the editor painted every tab orange).
		colors[ImGuiCol_Tab]                  = theme.Surface;
		colors[ImGuiCol_TabHovered]           = theme.AccentHovered;
		colors[ImGuiCol_TabActive]            = theme.Accent;
		colors[ImGuiCol_TabUnfocused]         = theme.WindowBg;
		colors[ImGuiCol_TabUnfocusedActive]   = theme.Surface;

		// Keep additional platform viewports square and opaque, as the editor does.
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	}

	// The active theme.
	const Theme& GetTheme()
	{
		return s_Theme;
	}
}
