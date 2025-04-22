#include "Layer.h"

namespace Tiles
{
	Layer::Layer(size_t width, size_t height, const std::string& name)
		: m_Name(name), m_Width(width), m_Height(height)
	{
		m_Tiles.resize(m_Width * m_Height);
		Clear();
	}

	void Layer::Clear()
	{
		for (Tile& tile : m_Tiles)
		{
			tile.Reset();
		}
	}
}
