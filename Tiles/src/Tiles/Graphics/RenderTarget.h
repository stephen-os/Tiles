#pragma once
#include <memory>
#include <string>
#include <cstdint>


#include "FrameBuffer.h"

#include <glm/glm.hpp>

namespace Tiles
{
	/// Off-screen render target backed by a FrameBuffer; drawing while bound
	/// renders into its color attachment, which can be sampled or saved out.
	class RenderTarget
	{
	public:
		static std::shared_ptr<RenderTarget> Create(uint32_t width, uint32_t height);

		RenderTarget(uint32_t width, uint32_t height);

		void Bind();
		void Unbind();

		void Resize(uint32_t width, uint32_t height);
		void Resize(float width, float height);
		void Resize(const glm::vec2& size);

		/// Reads back the color attachment and writes it to @p path. The image
		/// encoder is chosen from the extension (.png/.jpg/.jpeg/.bmp/.tga).
		/// Returns false on an unsupported extension or a write failure.
		bool SaveToFile(const std::string& path);

		uint32_t GetTexture() const { return m_FrameBuffer->GetColorAttachment(); }
		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

	private:
		std::shared_ptr<FrameBuffer> m_FrameBuffer;
		uint32_t m_Width;
		uint32_t m_Height;
	};
}
