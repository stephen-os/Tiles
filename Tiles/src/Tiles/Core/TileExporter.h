#pragma once

#include "TileLayer.h"
#include "Lumina/Renderer/TextureAtlas.h"

class TileExporter
{
public:
	int& GetResolution() { return m_Resolution; }
	void Export(TileLayer& layer, Lumina::TextureAtlas& atlas, std::string& filename);
private:
	int m_Resolution = 64;
};