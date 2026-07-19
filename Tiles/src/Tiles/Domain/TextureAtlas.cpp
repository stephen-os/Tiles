#include "Domain/TextureAtlas.h"

#include "Core/Logger.h"

#include <fstream>
#include <utility>

namespace Tiles
{
	namespace
	{
		// Reads an entire file into a byte buffer; returns false on failure.
		bool ReadFileBytes(const std::string& path, std::vector<uint8_t>& out)
		{
			std::ifstream file(path, std::ios::binary | std::ios::ate);
			if (!file)
				return false;

			std::streamsize size = file.tellg();
			if (size < 0)
				return false;

			out.resize(static_cast<size_t>(size));
			file.seekg(0);
			if (size > 0)
				file.read(reinterpret_cast<char*>(out.data()), size);

			return !file.bad();
		}
	}

	// Builds a textureless atlas with the given grid dimensions.
	std::shared_ptr<TextureAtlas> TextureAtlas::Create(int width, int height)
	{
		return std::make_shared<TextureAtlas>(width, height);
	}

	// Builds an atlas that loads its source image from a file.
	std::shared_ptr<TextureAtlas> TextureAtlas::Create(const std::string& imagePath, int width, int height)
	{
		return std::make_shared<TextureAtlas>(imagePath, width, height);
	}

	// Builds an atlas over already-encoded image bytes.
	std::shared_ptr<TextureAtlas> TextureAtlas::Create(std::vector<uint8_t> imageBytes, int width, int height)
	{
		return std::make_shared<TextureAtlas>(std::move(imageBytes), width, height);
	}

	// Creates a textureless atlas sized to the grid.
	TextureAtlas::TextureAtlas(int width, int height)
	{
		Resize(width, height);
	}

	// Creates an atlas, loading its source image from a file.
	TextureAtlas::TextureAtlas(const std::string& imagePath, int width, int height)
	{
		SetImage(imagePath);
		Resize(width, height);
	}

	// Creates an atlas over already-encoded image bytes.
	TextureAtlas::TextureAtlas(std::vector<uint8_t> imageBytes, int width, int height)
	{
		SetImageBytes(std::move(imageBytes));
		Resize(width, height);
	}

	// Sets the grid dimensions and precomputes the UV rectangle of every cell so
	// per-frame lookups are a plain array index.
	void TextureAtlas::Resize(int width, int height)
	{
		// A grid must be at least 1x1: a zero or negative dimension would divide by
		// zero when computing UVs (and in GetOffset).
		m_GridWidth = width > 0 ? width : 1;
		m_GridHeight = height > 0 ? height : 1;

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

	// Loads the source image from a file; leaves the atlas imageless on failure.
	void TextureAtlas::SetImage(const std::string& imagePath)
	{
		std::vector<uint8_t> bytes;
		if (!ReadFileBytes(imagePath, bytes))
		{
			TILES_ENGINE_ERROR("TextureAtlas::SetImage: failed to read image file '{}'", imagePath);
			return;
		}

		m_ImageBytes = std::move(bytes);
		m_SourcePath = imagePath;
		m_Version++;
	}

	// Adopts already-encoded image bytes as the source image.
	void TextureAtlas::SetImageBytes(std::vector<uint8_t> imageBytes)
	{
		m_ImageBytes = std::move(imageBytes);
		m_SourcePath.clear();
		m_Version++;
	}

	// Drops the source image, keeping the grid dimensions.
	void TextureAtlas::RemoveImage()
	{
		m_ImageBytes.clear();
		m_SourcePath.clear();
		m_Version++;
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
		if (index < 0 || m_GridWidth <= 0 || index >= m_GridWidth * m_GridHeight)
			return glm::vec2(0.0f);

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
