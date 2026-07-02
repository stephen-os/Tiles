#pragma once

#include <string>
#include <vector>

#include "Core/Base.h"

#include "Formats/TextureFormat.h"

namespace Tiles
{
	class Texture
	{
	public:
		static std::shared_ptr<Texture> Create(const std::string& source);
		static std::shared_ptr<Texture> Create(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA8);

		static std::shared_ptr<Texture> CreateFromData(const void* data, uint32_t width, uint32_t height, int components);
		static std::shared_ptr<Texture> CreateFromData(const void* data, uint32_t width, uint32_t height, TextureFormat format);

		static std::shared_ptr<Texture> CreateCubemap(const std::vector<std::string>& faces);
		static std::shared_ptr<Texture> CreateCubemap(uint32_t width, uint32_t height, const void* data);

		Texture(const std::string& source);
		Texture(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA8);
		~Texture();

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;

		void Bind(uint32_t slot = 0) const;
		void Unbind() const;

		bool SetResolution(uint32_t width, uint32_t height);
		void SetData(const void* data, uint32_t size);
		void SetData(const void* data, uint32_t width, uint32_t height, int components);
		void SetData(const void* data, uint32_t width, uint32_t height, TextureFormat format);

		uint32_t GetID() const { return m_BufferID; }
		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		TextureFormat GetFormat() const { return m_Format; }
		int GetComponentCount() const;
		const std::string& GetPath() const { return m_Path; }

		bool IsCubemap() const { return m_IsCubemap; }

	private:
		void LoadFromFile(const std::string& path);
		void CreateTexture(uint32_t width, uint32_t height, TextureFormat format, const void* data = nullptr);
		void CreateCubemapTexture(uint32_t width, uint32_t height, const void* data = nullptr);

	private:
		std::string m_Path = "";
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_BufferID = 0;
		TextureFormat m_Format = TextureFormat::RGBA8;
		bool m_IsCubemap = false;
	};
}
