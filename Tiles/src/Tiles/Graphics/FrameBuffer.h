#pragma once

#include <memory>
#include <cstdint>

namespace Tiles
{
	/// Offscreen render target: a GL framebuffer with a color texture and a depth
	/// renderbuffer. RAII wrapper owning those GL handles.
	class FrameBuffer
	{
	public:
		FrameBuffer();
		~FrameBuffer();

		// Owns GL framebuffer/texture/renderbuffer handles freed in the destructor;
		// copying or moving would risk a double free, and it is only ever held via
		// shared_ptr, so neither is permitted.
		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer& operator=(const FrameBuffer&) = delete;
		FrameBuffer(FrameBuffer&&) = delete;
		FrameBuffer& operator=(FrameBuffer&&) = delete;

		void Bind() const;
		void Unbind() const;

		void Resize(uint32_t width, uint32_t height);

		uint32_t GetID() const { return m_BufferID; }
		uint32_t GetColorAttachment() const { return m_ColorAttachment; }

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		void ReadPixels(int x, int y, uint32_t width, uint32_t height, void* data) const;

		static std::shared_ptr<FrameBuffer> Create();

	private:
		uint32_t m_BufferID = 0;
		uint32_t m_ColorAttachment = 0;
		uint32_t m_DepthAttachment = 0;

		uint32_t m_Width = 900;
		uint32_t m_Height = 900;
	};
}
