#pragma once

#include "../TileLayer.h"

#include "Lumina/Renderer/TextureAtlas.h"

class TileViewportPanel
{
public:
	void Render(TileLayer& layers, Lumina::TextureAtlas& atlas, int selectedTexture);
private:
	float m_Zoom = 1.0f;

	bool m_Fill = false;
	bool m_Erase = false;

	// Constants
	static constexpr float TILE_SIZE = 40.0f;
};