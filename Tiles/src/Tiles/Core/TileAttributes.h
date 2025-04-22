#pragma once

#include <glm/glm.hpp>

#include "Tile.h"

namespace Tiles
{
	class TileAttributes
	{
	public:
		void SetTextureIndex(int index) { m_TextureIndex = index; }
		int GetTextureIndex() const { return m_TextureIndex; }
		bool IsTextureSelected() const { return m_TextureIndex != -1; }

		glm::vec4& GetTintColor() { return m_TintColor; }

		void SetRotation(float rotation) { m_Rotation = rotation; }
		float GetRotation() const { return m_Rotation; }

		Tile GetTile() const
		{
			return Tile(m_TextureIndex, m_TintColor, m_Rotation);
		}
	private:
		int m_TextureIndex = -1; 
		glm::vec4 m_TintColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		float m_Rotation = 0.0f;
	};
}