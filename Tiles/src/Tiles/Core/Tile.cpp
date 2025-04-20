#include "Tile.h"

namespace Tiles
{
	void Tile::Reset()
	{
		m_TextureIndex = -1;
		m_TintColor = glm::vec4(1.0f);
	}

	bool Tile::operator==(const Tile& other) const
	{
		return m_TextureIndex == other.m_TextureIndex &&
			m_TintColor == other.m_TintColor;
	}

	Tile& Tile::operator=(const Tile& other)
	{
		if (this != &other)
		{
			m_TextureIndex = other.m_TextureIndex;
			m_TintColor = other.m_TintColor;
		}
		return *this;
	}
}
