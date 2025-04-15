#pragma once

#include "Base.h"

#include "Layers.h"
#include "TextureSelection.h"

#include "Lumina/Renderer/TextureAtlas.h"

#include "../Commands/CommandHistory.h"

namespace Tiles
{

	class Tools
	{
	public:
		static void Paint(Shared<Layers>& layers, Shared<Lumina::TextureAtlas>& atlas, Shared<TextureSelection>& selection, Shared<CommandHistory>& history, size_t y, size_t x);
		static void Erase(Shared<Layers>& layers, Shared<CommandHistory>& history, size_t y, size_t x);
		static void Fill(Shared<Layers>& layers, Shared<TextureSelection>& selection, Shared<CommandHistory>& history, size_t y, size_t x);
	};

}