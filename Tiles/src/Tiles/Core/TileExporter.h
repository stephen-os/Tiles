#pragma once

#include "TileLayer.h"
#include "Lumina/Renderer/TextureAtlas.h"

#include "Lumina/Base.h"

class TileExporter
{
public:
	int& GetResolution() { return m_Resolution; }
	void Export(Lumina::Ref<TileLayer>& layers, Lumina::Ref<Lumina::TextureAtlas>& atlas, std::string& filename);
private:
	int m_Resolution = 64;
};