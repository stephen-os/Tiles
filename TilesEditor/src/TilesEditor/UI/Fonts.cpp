#include "Fonts.h"

#include <filesystem>

namespace Tiles::UI
{
	static ImFont* s_Body = nullptr;
	static ImFont* s_Bold = nullptr;
	static float   s_BaseSize = 16.0f;

	// Loads body + bold fonts, falling back to the default; see header.
	void InitFonts(const FontConfig& config)
	{
		ImGuiIO& io = ImGui::GetIO();
		s_BaseSize = config.BaseSize;

		// Only load files that exist -- a missing path would otherwise assert.
		if (std::filesystem::exists(config.BodyPath))
			s_Body = io.Fonts->AddFontFromFileTTF(config.BodyPath, config.BaseSize);
		if (std::filesystem::exists(config.BoldPath))
			s_Bold = io.Fonts->AddFontFromFileTTF(config.BoldPath, config.BaseSize);

		// Guarantee a body font, and fall back to it for bold when there is no bold
		// file (emphasis then comes from size alone).
		if (!s_Body)
			s_Body = io.Fonts->AddFontDefault();
		if (!s_Bold)
			s_Bold = s_Body;

		io.FontDefault = s_Body;
	}

	// The font for a role; see header.
	ImFont* GetFont(FontRole role)
	{
		return role == FontRole::Bold ? s_Bold : s_Body;
	}

	// The base font size; see header.
	float GetBaseSize()
	{
		return s_BaseSize;
	}
}
