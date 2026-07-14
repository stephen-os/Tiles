#pragma once

#include "Formats/TextureFormat.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Tiles
{
	// A 2D texture; RAII wrapper owning a single GL texture object, allocated
	// with immutable storage via direct state access (DSA).
	class Texture
	{
	public:
		// Loads a texture from an image file on disk.
		[[nodiscard]] static std::shared_ptr<Texture> Create(const std::string& source);
		// Allocates an empty texture of the given size and format.
		[[nodiscard]] static std::shared_ptr<Texture> Create(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA8);

		// Uploads pixel data, deriving the format from the channel count.
		[[nodiscard]] static std::shared_ptr<Texture> CreateFromData(const void* data, uint32_t width, uint32_t height, int components);
		[[nodiscard]] static std::shared_ptr<Texture> CreateFromData(const void* data, uint32_t width, uint32_t height, TextureFormat format);

		Texture(const std::string& source);
		Texture(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA8);
		~Texture();

		// Owns a GL texture handle freed in the destructor; move-only so the
		// handle is never double-freed.
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;

		// Binds the texture to texture unit @p slot.
		void Bind(uint32_t slot = 0) const;
		void Unbind() const;

		// Recreates the texture at a new size, keeping the current format.
		bool SetResolution(uint32_t width, uint32_t height);

		// Uploads @p size bytes into the existing storage; @p size must match the
		// current dimensions and format exactly.
		void SetData(const void* data, uint32_t size);
		// Recreates the texture at the given size/format and uploads @p data.
		void SetData(const void* data, uint32_t width, uint32_t height, int components);
		void SetData(const void* data, uint32_t width, uint32_t height, TextureFormat format);

		[[nodiscard]] uint32_t GetID() const { return m_BufferID; }
		[[nodiscard]] uint32_t GetWidth() const { return m_Width; }
		[[nodiscard]] uint32_t GetHeight() const { return m_Height; }
		[[nodiscard]] TextureFormat GetFormat() const { return m_Format; }
		[[nodiscard]] int GetComponentCount() const;
		[[nodiscard]] const std::string& GetPath() const { return m_Path; }

		// Reads the texture back from the GPU as tightly-packed RGBA8 pixels
		// (width*height*4 bytes), so an atlas can be embedded in a saved project.
		[[nodiscard]] std::vector<uint8_t> ReadPixels() const;

	private:
		// Decodes an image file and uploads it into a fresh GL texture.
		void LoadFromFile(const std::string& path);
		// Allocates immutable GL storage and, if given, uploads initial pixels.
		void CreateTexture(uint32_t width, uint32_t height, TextureFormat format, const void* data = nullptr);

	private:
		std::string m_Path;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_BufferID = 0;
		TextureFormat m_Format = TextureFormat::RGBA8;
	};
}
