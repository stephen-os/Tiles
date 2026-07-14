#pragma once

#include <cstddef>
#include <cstdint>

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
}
