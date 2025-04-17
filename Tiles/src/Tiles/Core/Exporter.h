#pragma once

#include "Lumina/Renderer/TextureAtlas.h"

#include "Lumina/Core/Aliases.h"

#include "Layers.h"


namespace Tiles
{

	class Exporter
	{
	public:
		int& GetResolution() { return m_Resolution; }
		void Export(Shared<Layers>& layers, Shared<Lumina::TextureAtlas>& atlas, std::string& filepath, std::string& filename, std::vector<size_t>& groupings);
	private:
		int m_Resolution = 64;
	};

}