#include "Texture.h"

#include "Core/Assert.h"

#include <glad/gl.h>
#include <stb_image.h>

#include <utility>

namespace Tiles
{
	const TextureFormatInfo TextureFormats::s_FormatTable[] =
	{
		// Format       DataFormat  DataType            Components  Bytes   Name        IsFloat
		{ 0,            0,          0,                  0,          0,      "None",     false },  // None
		{ GL_R8,        GL_RED,     GL_UNSIGNED_BYTE,   1,          1,      "R8",       false },  // R8
		{ GL_RG8,       GL_RG,      GL_UNSIGNED_BYTE,   2,          2,      "RG8",      false },  // RG8
		{ GL_RGB8,      GL_RGB,     GL_UNSIGNED_BYTE,   3,          3,      "RGB8",     false },  // RGB8
		{ GL_RGBA8,     GL_RGBA,    GL_UNSIGNED_BYTE,   4,          4,      "RGBA8",    false },  // RGBA8
		{ GL_R16F,      GL_RED,     GL_FLOAT,           1,          2,      "R16F",     true  },  // R16F
		{ GL_RG16F,     GL_RG,      GL_FLOAT,           2,          4,      "RG16F",    true  },  // RG16F
		{ GL_RGB16F,    GL_RGB,     GL_FLOAT,           3,          6,      "RGB16F",   true  },  // RGB16F
		{ GL_RGBA16F,   GL_RGBA,    GL_FLOAT,           4,          8,      "RGBA16F",  true  },  // RGBA16F
		{ GL_R32F,      GL_RED,     GL_FLOAT,           1,          4,      "R32F",     true  },  // R32F
		{ GL_RG32F,     GL_RG,      GL_FLOAT,           2,          8,      "RG32F",    true  },  // RG32F
		{ GL_RGB32F,    GL_RGB,     GL_FLOAT,           3,          12,     "RGB32F",   true  },  // RGB32F
		{ GL_RGBA32F,   GL_RGBA,    GL_FLOAT,           4,          16,     "RGBA32F",  true  },  // RGBA32F
	};

	// The info record for format, or the None record if out of range.
	const TextureFormatInfo& TextureFormats::GetInfo(TextureFormat format)
	{
		uint8_t index = static_cast<uint8_t>(format);

		if (index >= sizeof(s_FormatTable) / sizeof(s_FormatTable[0]))
			return s_FormatTable[0];

		return s_FormatTable[index];
	}

	// Maps a channel count (1-4) to the matching 8-bit format, None otherwise.
	TextureFormat TextureFormats::FromComponentCount(int components)
	{
		switch (components)
		{
		case 1: return TextureFormat::R8;
		case 2: return TextureFormat::RG8;
		case 3: return TextureFormat::RGB8;
		case 4: return TextureFormat::RGBA8;
		default: return TextureFormat::None;
		}
	}

	// True for a real (non-None, in-range) format.
	bool TextureFormats::IsValidFormat(TextureFormat format)
	{
		return format != TextureFormat::None && static_cast<uint8_t>(format) < (sizeof(s_FormatTable) / sizeof(s_FormatTable[0]));
	}

	// The byte size of a width x height image in the given format.
	size_t TextureFormats::CalculateImageSize(TextureFormat format, uint32_t width, uint32_t height)
	{
		return static_cast<size_t>(width) * height * GetBytesPerPixel(format);
	}

	// Loads a texture from an image file on disk.
	std::shared_ptr<Texture> Texture::Create(const std::string& source)
	{
		return std::make_shared<Texture>(source);
	}

	// Allocates an empty texture of the given size and format.
	std::shared_ptr<Texture> Texture::Create(uint32_t width, uint32_t height, TextureFormat format)
	{
		return std::make_shared<Texture>(width, height, format);
	}

	// Creates a texture and uploads data, deriving the format from the channel count.
	std::shared_ptr<Texture> Texture::CreateFromData(const void* data, uint32_t width, uint32_t height, int components)
	{
		TextureFormat format = TextureFormats::FromComponentCount(components);
		if (format == TextureFormat::None)
		{
			TILES_ENGINE_ERROR("Unsupported component count: {0}", components);
			return nullptr;
		}

		auto texture = Texture::Create(width, height, format);
		if (data)
			texture->SetData(data, width, height, format);

		return texture;
	}

	// Creates a texture and uploads data in the given format.
	std::shared_ptr<Texture> Texture::CreateFromData(const void* data, uint32_t width, uint32_t height, TextureFormat format)
	{
		auto texture = Texture::Create(width, height, format);
		if (data)
			texture->SetData(data, width, height, format);

		return texture;
	}

	// Creates a texture by decoding an image file.
	Texture::Texture(const std::string& source)
	{
		LoadFromFile(source);
	}

	// Creates an empty texture with immutable storage at the given size/format.
	Texture::Texture(uint32_t width, uint32_t height, TextureFormat format)
		: m_Width(width), m_Height(height), m_Format(format)
	{
		CreateTexture(width, height, format);
	}

	// Deletes the GL texture (a no-op for a moved-from id of 0).
	Texture::~Texture()
	{
		glDeleteTextures(1, &m_BufferID);
	}

	// Steals other's handle, leaving it id 0.
	Texture::Texture(Texture&& other) noexcept
		: m_Path(std::move(other.m_Path)), m_Width(other.m_Width), m_Height(other.m_Height),
		  m_BufferID(other.m_BufferID), m_Format(other.m_Format)
	{
		other.m_BufferID = 0;
	}

	// Frees this handle, then steals other's.
	Texture& Texture::operator=(Texture&& other) noexcept
	{
		if (this != &other)
		{
			glDeleteTextures(1, &m_BufferID);
			m_Path = std::move(other.m_Path);
			m_Width = other.m_Width;
			m_Height = other.m_Height;
			m_BufferID = other.m_BufferID;
			m_Format = other.m_Format;
			other.m_BufferID = 0;
		}
		return *this;
	}

	// Binds the texture to texture unit slot.
	void Texture::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_BufferID);
	}

	// With DSA, explicit unbinding is unnecessary; kept for call-site symmetry.
	void Texture::Unbind() const
	{
	}

	// Recreates the texture at a new size, keeping the current format.
	bool Texture::SetResolution(uint32_t width, uint32_t height)
	{
		TILES_ASSERT(width > 0 && height > 0, "Invalid resolution: {0}, {1}", width, height);

		m_Width = width;
		m_Height = height;

		glDeleteTextures(1, &m_BufferID);
		CreateTexture(width, height, m_Format);

		return true;
	}

	// Uploads size bytes into the existing storage; size must match the current
	// dimensions and format exactly.
	void Texture::SetData(const void* data, uint32_t size)
	{
		TILES_ASSERT(data != nullptr, "SetData called with null data pointer");

		uint32_t expectedSize = m_Width * m_Height * TextureFormats::GetBytesPerPixel(m_Format);
		TILES_ASSERT(size == expectedSize, "Texture::SetData - Data size mismatch. Expected: {0}, got: {1}", expectedSize, size);

		auto formatInfo = TextureFormats::GetInfo(m_Format);
		glTextureSubImage2D(m_BufferID, 0, 0, 0, m_Width, m_Height, formatInfo.DataFormat, formatInfo.DataType, data);
	}

	// Recreates the texture at the given size and channel count, then uploads data.
	void Texture::SetData(const void* data, uint32_t width, uint32_t height, int components)
	{
		TextureFormat format = TextureFormats::FromComponentCount(components);
		SetData(data, width, height, format);
	}

	// Recreates the texture at the given size/format and uploads data.
	void Texture::SetData(const void* data, uint32_t width, uint32_t height, TextureFormat format)
	{
		if (!data)
		{
			TILES_ENGINE_ERROR("Cannot set null data to texture");
			return;
		}

		m_Width = width;
		m_Height = height;
		m_Format = format;

		auto formatInfo = TextureFormats::GetInfo(format);

		// Immutable storage cannot be resized or reformatted in place, so a new
		// size/format requires deleting and recreating the texture object.
		glDeleteTextures(1, &m_BufferID);
		glCreateTextures(GL_TEXTURE_2D, 1, &m_BufferID);
		glTextureStorage2D(m_BufferID, 1, formatInfo.InternalFormat, width, height);
		glTextureSubImage2D(m_BufferID, 0, 0, 0, width, height, formatInfo.DataFormat, formatInfo.DataType, data);

		glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(m_BufferID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_BufferID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenerateTextureMipmap(m_BufferID);
	}

	// The number of channels in the current format.
	int Texture::GetComponentCount() const
	{
		return TextureFormats::GetComponentCount(m_Format);
	}

	// Reads the texture back from the GPU as tightly-packed RGBA8 pixels.
	std::vector<uint8_t> Texture::ReadPixels() const
	{
		std::vector<uint8_t> pixels(static_cast<size_t>(m_Width) * m_Height * 4);
		glGetTextureImage(m_BufferID, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			static_cast<GLsizei>(pixels.size()), pixels.data());
		return pixels;
	}

	// Decodes an image file with stb_image and uploads it into a fresh texture.
	void Texture::LoadFromFile(const std::string& path)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_BufferID);

		int channels;
		int width, height;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		TILES_ASSERT(data, "Failed to load texture: {0}", path);

		m_Width = width;
		m_Height = height;
		m_Path = path;
		m_Format = TextureFormats::FromComponentCount(channels);

		auto formatInfo = TextureFormats::GetInfo(m_Format);

		glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(m_BufferID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_BufferID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureStorage2D(m_BufferID, 1, formatInfo.InternalFormat, m_Width, m_Height);
		glTextureSubImage2D(m_BufferID, 0, 0, 0, m_Width, m_Height, formatInfo.DataFormat, GL_UNSIGNED_BYTE, data);

		glGenerateTextureMipmap(m_BufferID);

		stbi_image_free(data);
	}

	// Allocates immutable GL storage and, if given, uploads initial pixels.
	void Texture::CreateTexture(uint32_t width, uint32_t height, TextureFormat format, const void* data)
	{
		TILES_ASSERT(width > 0 && height > 0, "Texture dimensions must be greater than zero");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_BufferID);

		auto formatInfo = TextureFormats::GetInfo(format);

		glTextureStorage2D(m_BufferID, 1, formatInfo.InternalFormat, width, height);

		if (data)
			glTextureSubImage2D(m_BufferID, 0, 0, 0, width, height, formatInfo.DataFormat, formatInfo.DataType, data);

		glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(m_BufferID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_BufferID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}
