#pragma once

#include <glm/glm.hpp>

namespace Tiles
{
	class TileAttributes
	{
	public:
		int GetTextureIndex() const { return m_TextureIndex; }
		void SetTextureIndex(int index) { m_TextureIndex = index; }

		glm::vec4& GetTintColor() { return m_TintColor; }

	private:
		int m_TextureIndex = -1; 
		glm::vec4 m_TintColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};
}