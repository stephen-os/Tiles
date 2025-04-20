#pragma once

#include "../Core/TileAttributes.h"
#include "Lumina/Core/Aliases.h"

namespace Tiles
{
	class TintSelectionPanel
	{
	public:
		TintSelectionPanel() = default;
		~TintSelectionPanel() = default;

		void OnUIRender();

		void SetTileAttributes(const Shared<TileAttributes>& tileAttributes) { m_TileAttributes = tileAttributes; }
	private:
		Shared<TileAttributes> m_TileAttributes;
	};
}
