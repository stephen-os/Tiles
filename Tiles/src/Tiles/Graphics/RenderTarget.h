#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace Tiles
{
	// Off-screen render target: a GL framebuffer with a color texture and a depth
	// renderbuffer. Drawing while bound renders into the color attachment, which
	// can be sampled (GetTexture) or saved to disk (SaveToFile). RAII wrapper
	// owning the GL handles; only ever held via shared_ptr, so it is neither
	// copyable nor movable.
	class RenderTarget
	{
	public:
		[[nodiscard]] static std::shared_ptr<RenderTarget> Create(uint32_t width, uint32_t height);

		RenderTarget(uint32_t width, uint32_t height);
		~RenderTarget();

		RenderTarget(const RenderTarget&) = delete;
		RenderTarget& operator=(const RenderTarget&) = delete;
		RenderTarget(RenderTarget&&) = delete;
		RenderTarget& operator=(RenderTarget&&) = delete;

		// Makes this the current draw target.
		void Bind() const;
		// Restores the default (screen) framebuffer.
		void Unbind() const;

		void Resize(uint32_t width, uint32_t height);
		void Resize(float width, float height);
		void Resize(const glm::vec2& size);

		// Reads back the color attachment and writes it to @p path. The image
		// encoder is chosen from the extension (.png/.jpg/.jpeg/.bmp/.tga).
		// Returns false on an unsupported extension or a write failure.
		bool SaveToFile(const std::string& path);

		// The GL id of the color attachment texture (e.g. for ImGui display).
		[[nodiscard]] uint32_t GetTexture() const { return m_ColorAttachment; }
		[[nodiscard]] uint32_t GetWidth() const { return m_Width; }
		[[nodiscard]] uint32_t GetHeight() const { return m_Height; }

	private:
		// (Re)allocates the color texture and depth renderbuffer at the current
		// size and validates framebuffer completeness.
		void AllocateAttachments();

		uint32_t m_BufferID = 0;
		uint32_t m_ColorAttachment = 0;
		uint32_t m_DepthAttachment = 0;

		uint32_t m_Width;
		uint32_t m_Height;
	};
}
