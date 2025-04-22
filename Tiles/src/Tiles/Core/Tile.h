#pragma once

#include "glm/glm.hpp"

namespace Tiles
{
	struct TilePosition
	{
		size_t LayerIndex;
		size_t RowIndex;
		size_t ColIndex;

		TilePosition() = default;

		TilePosition(size_t layerIndex, size_t rowIndex, size_t colIndex)
			: LayerIndex(layerIndex), RowIndex(rowIndex), ColIndex(colIndex) {}

		bool operator==(const TilePosition& other) const
		{
			return LayerIndex == other.LayerIndex && RowIndex == other.RowIndex && ColIndex == other.ColIndex;
		}
	};

	class Tile
	{
	public:
		Tile() = default;
		Tile(int textureIndex, const glm::vec4& tintColor, const float rotation)
			: m_TextureIndex(textureIndex), m_TintColor(tintColor), m_Rotation(rotation) {
		}
		~Tile() = default;

		void Reset();

		// Setters
		void SetTextureIndex(int textureIndex) { m_TextureIndex = textureIndex; }
		void SetTintColor(const glm::vec4& color) { m_TintColor = color; }
		void SetRotation(float rotation) { m_Rotation = rotation; }

		// Getters
		int GetTextureIndex() const { return m_TextureIndex; }
		const glm::vec4& GetTintColor() const { return m_TintColor; }
		float GetRotation() const { return m_Rotation; }

		bool UseTexture() const { return m_TextureIndex != -1; }

		// Operator
		bool operator==(const Tile& other) const;
		Tile& Tile::operator=(const Tile& other);
	private:
		int m_TextureIndex = -1;
		float m_Rotation = 0.0f;
		glm::vec4 m_TintColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};
}