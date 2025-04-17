#pragma once


#include "Layers.h"
#include "TextureSelection.h"

#include "Lumina/Renderer/TextureAtlas.h"
#include "Lumina/Core/Aliases.h"

#include "../Commands/CommandHistory.h"

namespace Tiles
{
	enum class ToolMode
	{
		None = 0,
		Paint,
		Erase,
		Fill
	};

	class Tools
	{
	public:
		static ToolMode GetCurrentMode() { return s_CurrentMode; }
		static void SetCurrentMode(ToolMode mode) { s_CurrentMode = mode; }

		static void Paint(Shared<Layers>& layers, Shared<Lumina::TextureAtlas>& atlas, Shared<TextureSelection>& selection, Shared<CommandHistory>& history, size_t y, size_t x);
		static void Erase(Shared<Layers>& layers, Shared<CommandHistory>& history, size_t y, size_t x);
		static void Fill(Shared<Layers>& layers, Shared<TextureSelection>& selection, Shared<CommandHistory>& history, size_t y, size_t x);
	private: 
		static inline ToolMode s_CurrentMode = ToolMode::None;
	};
}