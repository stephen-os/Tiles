#pragma once

#include "glm/glm.hpp"

namespace Tiles
{
	struct TilePosition
	{
		size_t LayerIndex;
		size_t RowIndex;
		size_t ColIndex;

		bool operator==(const TilePosition& other) const
		{
			return LayerIndex == other.LayerIndex && RowIndex == other.RowIndex && ColIndex == other.ColIndex;
		}
	};

	class Tile
	{
	public:
		Tile() = default;
		~Tile() = default;

		void Reset();

		// Setters
		void SetTextureIndex(int textureIndex) { m_TextureIndex = textureIndex; }

		// Getters
		int GetTextureIndex() const { return m_TextureIndex; }
		bool UseTexture() const { return m_TextureIndex != -1; }

		// Operator
		bool operator==(const Tile& other) const;
		Tile& Tile::operator=(const Tile& other);
	private:
		int m_TextureIndex = -1;
	};
}