#include "TextureAtlas.h"

#include "Core/Logger.h"

namespace Tiles
{
	// Builds an atlas that loads its texture from a file.
	std::shared_ptr<TextureAtlas> TextureAtlas::Create(const std::string& source, int width, int height)
	{
		return std::make_shared<TextureAtlas>(source, width, height);
	}

	// Builds a textureless atlas with the given grid dimensions.
	std::shared_ptr<TextureAtlas> TextureAtlas::Create(int width, int height)
	{
		return std::make_shared<TextureAtlas>(width, height);
	}

	// Builds an atlas over an already-created texture.
	std::shared_ptr<TextureAtlas> TextureAtlas::Create(std::shared_ptr<Texture> texture, int width, int height)
	{
		return std::make_shared<TextureAtlas>(std::move(texture), width, height);
	}

	// Creates a textureless atlas sized to the grid.
	TextureAtlas::TextureAtlas(int width, int height)
	{
		Resize(width, height);
	}

	// Creates an atlas, loading its texture from a file.
	TextureAtlas::TextureAtlas(const std::string& source, int width, int height)
	{
		SetTexture(source);
		Resize(width, height);
	}

	// Creates an atlas over an existing texture.
	TextureAtlas::TextureAtlas(std::shared_ptr<Texture> texture, int width, int height)
	{
		SetTexture(std::move(texture));
		Resize(width, height);
	}

	// Sets the grid dimensions and precomputes the UV rectangle of every cell
	// so per-frame lookups are a plain array index.
	void TextureAtlas::Resize(int width, int height)
	{
		m_GridWidth = width;
		m_GridHeight = height;

		m_TexWidth = 1.0f / static_cast<float>(m_GridWidth);
		m_TexHeight = 1.0f / static_cast<float>(m_GridHeight);

		m_TexCoords.clear();
		m_TexCoords.reserve(m_GridWidth * m_GridHeight);

		for (int y = 0; y < m_GridHeight; y++)
		{
			for (int x = 0; x < m_GridWidth; x++)
			{
				float uMin = x * m_TexWidth;
				float vMin = y * m_TexHeight;
				float uMax = uMin + m_TexWidth;
				float vMax = vMin + m_TexHeight;

				m_TexCoords.emplace_back(uMin, vMin, uMax, vMax);
			}
		}
	}

	// Loads the backing texture from a file.
	void TextureAtlas::SetTexture(const std::string& source)
	{
		m_Texture = Texture::Create(source);
		m_HasTexture = true;
	}

	// Replaces the backing texture with an existing one.
	void TextureAtlas::SetTexture(std::shared_ptr<Texture> texture)
	{
		m_Texture = std::move(texture);
		m_HasTexture = (m_Texture != nullptr);
	}

	// Drops the backing texture, keeping the grid dimensions.
	void TextureAtlas::RemoveTexture()
	{
		m_Texture.reset();
		m_HasTexture = false;
	}

	// UV rectangle of a cell, or a zero vector if the index is out of range.
	glm::vec4 TextureAtlas::GetTextureCoords(int index) const
	{
		if (index < 0 || index >= static_cast<int>(m_TexCoords.size()))
		{
			TILES_ENGINE_ERROR("[Texture Atlas] Invalid texture index: {}", index);
			return glm::vec4(0.0f);
		}

		return m_TexCoords[index];
	}

	// UV-space offset of a cell's lower-left corner.
	glm::vec2 TextureAtlas::GetOffset(int index) const
	{
		int row = index / m_GridWidth;
		int col = index % m_GridWidth;

		float xOffset = col * m_TexWidth;
		float yOffset = row * m_TexHeight;

		return glm::vec2(xOffset, yOffset);
	}

	// Grid (column, row) coordinates of a cell, or zero if out of range.
	glm::vec2 TextureAtlas::GetPosition(int index) const
	{
		if (index < 0 || index >= m_GridWidth * m_GridHeight)
			return glm::vec2(0.0f);

		int x = index % m_GridWidth;
		int y = index / m_GridWidth;

		return glm::vec2(x, y);
	}
}
