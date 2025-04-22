#pragma once

#include "../Core/TileAttributes.h"
#include "Lumina/Core/Aliases.h"

namespace Tiles
{
	class AttributeSelectionPanel
	{
	public:
		AttributeSelectionPanel() = default;
		~AttributeSelectionPanel() = default;

		void Render();

		void SetTileAttributes(const Shared<TileAttributes>& tileAttributes) { m_TileAttributes = tileAttributes; }
	private:
		Shared<TileAttributes> m_TileAttributes;

		bool m_RotationLocked = true;
	};
}
