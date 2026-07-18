#pragma once

#include "AtlasId.h"

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Tiles
{
	// A uniform grid over an image, mapping cell indices to UV rectangles so tiles
	// can reference sub-images by index. Pure document data: it owns the source
	// image as raw encoded bytes (never a GL texture), so it stays free of the
	// graphics layer and serializes self-contained. The GPU texture is built and
	// cached on the view side, keyed off GetVersion().
	class TextureAtlas
	{
	public:
		[[nodiscard]] static std::shared_ptr<TextureAtlas> Create(int width, int height);
		// Loads the source image from a file on disk.
		[[nodiscard]] static std::shared_ptr<TextureAtlas> Create(const std::string& imagePath, int width, int height);
		// Adopts already-encoded image bytes (e.g. extracted from a saved project).
		[[nodiscard]] static std::shared_ptr<TextureAtlas> Create(std::vector<uint8_t> imageBytes, int width, int height);

		TextureAtlas(int width, int height);
		TextureAtlas(const std::string& imagePath, int width, int height);
		TextureAtlas(std::vector<uint8_t> imageBytes, int width, int height);
		~TextureAtlas() = default;

		// Sets the grid dimensions and recomputes every cell's UV rectangle.
		void Resize(int width, int height);

		// Loads (or replaces) the source image from a file; leaves the atlas
		// imageless on a read failure.
		void SetImage(const std::string& imagePath);
		// Adopts already-encoded image bytes as the source image.
		void SetImageBytes(std::vector<uint8_t> imageBytes);
		// Drops the source image, keeping the grid dimensions.
		void RemoveImage();

		[[nodiscard]] int GetWidth() const { return m_GridWidth; }
		[[nodiscard]] int GetHeight() const { return m_GridHeight; }

		// The atlas's stable per-project id. Assigned by Project::AddTextureAtlas;
		// Invalid until then.
		[[nodiscard]] AtlasId GetId() const { return m_Id; }
		void SetId(AtlasId id) { m_Id = id; }

		// A free-form display label (empty until set; the editor falls back to a
		// positional "Atlas N"). This is a label, not an identity -- that is GetId().
		[[nodiscard]] const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		[[nodiscard]] bool HasImage() const { return !m_ImageBytes.empty(); }
		// The raw encoded source image bytes (empty when the atlas has no image).
		[[nodiscard]] const std::vector<uint8_t>& GetImageBytes() const { return m_ImageBytes; }
		// The path the image was last loaded from, if any (empty for embedded bytes).
		[[nodiscard]] const std::string& GetSourcePath() const { return m_SourcePath; }
		// Bumped whenever the source image changes, so a texture cache can tell when
		// to rebuild.
		[[nodiscard]] uint64_t GetVersion() const { return m_Version; }

		// UV rectangle of cell @p index, packed as (uMin, vMin, uMax, vMax).
		// Returns a zero vector on an out-of-range index.
		[[nodiscard]] glm::vec4 GetTextureCoords(int index) const;
		// UV-space offset of cell @p index's lower-left corner.
		[[nodiscard]] glm::vec2 GetOffset(int index) const;
		// Grid (column, row) coordinates of cell @p index.
		[[nodiscard]] glm::vec2 GetPosition(int index) const;

	private:
		std::vector<uint8_t> m_ImageBytes;
		std::string m_SourcePath;
		int m_GridWidth = 1;                    // cells across
		int m_GridHeight = 1;                   // cells down
		float m_TexWidth = 1.0f;                // one cell's width in UV space
		float m_TexHeight = 1.0f;               // one cell's height in UV space
		std::vector<glm::vec4> m_TexCoords;     // per-cell UV rects, filled by Resize
		uint64_t m_Version = 0;
		AtlasId m_Id = AtlasId::Invalid;
		std::string m_Name;                     // display label; empty => positional fallback
	};
}
