#pragma once

#include "../Core/TileLayer.h"
#include "../Core/TileExporter.h"

#include "Lumina/Renderer/TextureAtlas.h"

class HeaderPanel
{
public:
	void Render();

	void SetTextureAtlas(Lumina::TextureAtlas& atlas) { m_TextureAtlas = &atlas; }
	void SetTileLayer(TileLayer& layers) { m_Layers = &layers; }
private:
	void RenderFile();
	void RenderEdit();
	void RenderOptions(); 

	void RenderNewPopup();

private:
	TileLayer* m_Layers = nullptr;
	Lumina::TextureAtlas* m_TextureAtlas = nullptr;

	// New
	bool m_ShowNewPopup = false;
	int m_NewWidth = 10;
	int m_NewHeight = 10;
};