#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Tiles
{
	enum class TextureFormat : uint8_t
	{
		None = 0,
		R8, RG8, RGB8, RGBA8,
		R16F, RG16F, RGB16F, RGBA16F,
		R32F, RG32F, RGB32F, RGBA32F
	};

	struct TextureFormatInfo
	{
		uint32_t InternalFormat;
		uint32_t DataFormat;
		uint32_t DataType;
		uint8_t ComponentCount;
		uint8_t BytesPerPixel;
		const char* Name;
		bool IsFloat;
	};

	// Static lookup of per-format GL enums and pixel metrics, table-driven by
	// the TextureFormat enum value.
	class TextureFormats
	{
	public:
		// The info record for format, or the None record if out of range.
		[[nodiscard]] static const TextureFormatInfo& GetInfo(TextureFormat format);

		// The GL sized internal format.
		[[nodiscard]] static uint32_t GetInternalFormat(TextureFormat format) { return GetInfo(format).InternalFormat; }

		// The GL pixel data format (GL_RED/GL_RG/...).
		[[nodiscard]] static uint32_t GetDataFormat(TextureFormat format) { return GetInfo(format).DataFormat; }

		// The GL pixel data type (GL_UNSIGNED_BYTE/GL_FLOAT).
		[[nodiscard]] static uint32_t GetDataType(TextureFormat format) { return GetInfo(format).DataType; }

		// The number of channels.
		[[nodiscard]] static uint8_t GetComponentCount(TextureFormat format) { return GetInfo(format).ComponentCount; }

		// The bytes occupied by one pixel.
		[[nodiscard]] static uint8_t GetBytesPerPixel(TextureFormat format) { return GetInfo(format).BytesPerPixel; }

		// The format's display name.
		[[nodiscard]] static const char* GetName(TextureFormat format) { return GetInfo(format).Name; }

		// True for floating-point formats.
		[[nodiscard]] static bool IsFloat(TextureFormat format) { return GetInfo(format).IsFloat; }

		// Maps a channel count (1-4) to the matching 8-bit format, None otherwise.
		[[nodiscard]] static TextureFormat FromComponentCount(int components);

		// True for a real (non-None, in-range) format.
		[[nodiscard]] static bool IsValidFormat(TextureFormat format);

		// The byte size of a width x height image in the given format.
		[[nodiscard]] static size_t CalculateImageSize(TextureFormat format, uint32_t width, uint32_t height);

	private:
		static const TextureFormatInfo s_FormatTable[];
	};

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

		// Decodes an encoded image (PNG/JPG/...) from memory and uploads it.
		[[nodiscard]] static std::shared_ptr<Texture> CreateFromEncodedImage(const void* data, size_t size);

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
