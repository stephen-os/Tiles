#pragma once

#include "../Core/TileAttributes.h"
#include "../Core/Tile.h"

#include "Lumina/Core/Aliases.h"
#include "Lumina/Renderer/TextureAtlas.h"

#include "Lumina/Renderer/Cameras/OrthographicCamera.h"

namespace Tiles
{
	class TilePreviewPanel
	{
	public:
		TilePreviewPanel() = default;
		~TilePreviewPanel() = default;

		void OnUIRender();

		void SetTileAttributes(Shared<TileAttributes>& tileAttributes) { m_TileAttributes = tileAttributes; }
		void SetTextureAtlas(Shared<Lumina::TextureAtlas>& atlas) { m_Atlas = atlas; }
	private:
		Shared<TileAttributes> m_TileAttributes;
		Shared<Lumina::TextureAtlas> m_Atlas;

		Lumina::OrthographicCamera m_Camera;
	};
}