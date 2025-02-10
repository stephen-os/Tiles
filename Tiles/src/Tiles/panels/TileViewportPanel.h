#pragma once

#include "../TileLayer.h"

#include "Lumina/Renderer/TextureAtlas.h"

class TileViewportPanel
{
public:
	void Render(int selectedTexture);

	void SetTextureAtlas(Lumina::TextureAtlas& atlas) { m_TextureAtlas = &atlas; }
	void SetTileLayer(TileLayer& layers) { m_Layers = &layers; }
private:
	float m_Zoom = 1.0f;

	TileLayer* m_Layers = nullptr;
	Lumina::TextureAtlas* m_TextureAtlas = nullptr;

	bool m_Fill = false;
	bool m_Erase = false;

	// Constants
	static constexpr float TILE_SIZE = 40.0f;
};