#pragma once

#include "../Core/TileLayer.h"
#include "../Core/TileExporter.h"

#include "Lumina/Renderer/TextureAtlas.h"
#include "Lumina/Base.h"

class HeaderPanel
{
public:
	void Render();

	void SetTextureAtlas(Lumina::TextureAtlas& atlas) { m_TextureAtlas = &atlas; }
	void SetTileLayer(const Lumina::Ref<TileLayer>& layers) { m_Layers = layers; }
private:
	void RenderFile();
	void RenderEdit();
	void RenderOptions(); 

	void RenderNewPopup();

private:
	Lumina::Ref<TileLayer> m_Layers;
	Lumina::TextureAtlas* m_TextureAtlas = nullptr;

	// New
	bool m_ShowNewPopup = false;
	int m_NewWidth = 10;
	int m_NewHeight = 10;
};