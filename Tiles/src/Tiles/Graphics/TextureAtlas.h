#pragma once

#include "Texture.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

namespace Tiles
{
	// A uniform grid over a single texture: maps cell indices to UV rectangles
	// so tiles can reference sub-images by index. Cell UVs are precomputed on
	// resize, making per-frame lookups a plain array index.
	class TextureAtlas
	{
	public:
		[[nodiscard]] static std::shared_ptr<TextureAtlas> Create(const std::string& source, int width, int height);
		[[nodiscard]] static std::shared_ptr<TextureAtlas> Create(int width, int height);
		// Builds an atlas over an already-created texture (e.g. one decoded from
		// an embedded image on load).
		[[nodiscard]] static std::shared_ptr<TextureAtlas> Create(std::shared_ptr<Texture> texture, int width, int height);

		TextureAtlas(int width, int height);
		TextureAtlas(const std::string& source, int width, int height);
		TextureAtlas(std::shared_ptr<Texture> texture, int width, int height);
		~TextureAtlas() = default;

		// Sets the grid dimensions and recomputes every cell's UV rectangle.
		void Resize(int width, int height);

		// Loads (or replaces) the backing texture from a file.
		void SetTexture(const std::string& source);
		// Replaces the backing texture with an existing one.
		void SetTexture(std::shared_ptr<Texture> texture);
		// Drops the backing texture, keeping the grid dimensions.
		void RemoveTexture();

		[[nodiscard]] int GetWidth() const { return m_GridWidth; }
		[[nodiscard]] int GetHeight() const { return m_GridHeight; }

		[[nodiscard]] bool HasTexture() const { return m_HasTexture; }
		[[nodiscard]] const std::shared_ptr<Texture>& GetTexture() const { return m_Texture; }

		// UV rectangle of cell @p index, packed as (uMin, vMin, uMax, vMax).
		// Returns a zero vector on an out-of-range index.
		[[nodiscard]] glm::vec4 GetTextureCoords(int index) const;
		// UV-space offset of cell @p index's lower-left corner.
		[[nodiscard]] glm::vec2 GetOffset(int index) const;
		// Grid (column, row) coordinates of cell @p index.
		[[nodiscard]] glm::vec2 GetPosition(int index) const;

	private:
		std::shared_ptr<Texture> m_Texture;
		int m_GridWidth = 1;                    // cells across
		int m_GridHeight = 1;                   // cells down
		float m_TexWidth = 1.0f;                // one cell's width in UV space
		float m_TexHeight = 1.0f;               // one cell's height in UV space
		bool m_HasTexture = false;
		std::vector<glm::vec4> m_TexCoords;     // per-cell UV rects, filled by Resize
	};
}
