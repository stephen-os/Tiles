#include "Tile.h"

namespace Tiles
{

	void Tile::Reset()
	{
		m_TextureIndex = -1;
	}

	bool Tile::operator==(const Tile& other) const
	{
		return m_TextureIndex == other.m_TextureIndex;
	}

}