#pragma once

#include "../Core/Layers.h"
#include "../Core/TileExporter.h"
#include "../Core/Atlas.h"

#include "Lumina/Base.h"

class HeaderPanel
{
public:
	void Render();

	void SetTextureAtlas(const Lumina::Ref<Atlas>& atlas) { m_Atlas = atlas; }
	void SetTileLayers(const Lumina::Ref<Layers>& layers) { m_Layers = layers; }
private:
	void RenderFile();
	void RenderEdit();
	void RenderOptions(); 

	void RenderNewPopup();

private:
	Lumina::Ref<Layers> m_Layers;
	Lumina::Ref<Atlas> m_Atlas;

	// New
	bool m_ShowNewPopup = false;
	int m_NewWidth = 10;
	int m_NewHeight = 10;
};