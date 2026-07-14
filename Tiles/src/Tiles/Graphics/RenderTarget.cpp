#include "RenderTarget.h"

#include "Core/Assert.h"
#include "Core/Logger.h"

#include <glad/gl.h>
#include <stb_image_write.h>

#include <algorithm>
#include <vector>

namespace Tiles
{
	// Creates an offscreen render target of the given size.
	std::shared_ptr<RenderTarget> RenderTarget::Create(uint32_t width, uint32_t height)
	{
		return std::make_shared<RenderTarget>(width, height);
	}

	// Generates the framebuffer and attachment handles, then allocates them.
	RenderTarget::RenderTarget(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		glGenFramebuffers(1, &m_BufferID);
		TILES_ASSERT(m_BufferID != 0, "Failed to generate framebuffer");

		glGenTextures(1, &m_ColorAttachment);
		TILES_ASSERT(m_ColorAttachment != 0, "Failed to generate color attachment texture");

		glGenRenderbuffers(1, &m_DepthAttachment);
		TILES_ASSERT(m_DepthAttachment != 0, "Failed to generate depth renderbuffer");

		AllocateAttachments();
	}

	// Deletes the framebuffer and its attachments.
	RenderTarget::~RenderTarget()
	{
		glDeleteFramebuffers(1, &m_BufferID);
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteRenderbuffers(1, &m_DepthAttachment);
	}

	// Makes this framebuffer the current draw target.
	void RenderTarget::Bind() const
	{
		TILES_ASSERT(m_BufferID != 0, "Cannot bind an uninitialized render target");
		glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
	}

	// Restores the default (screen) framebuffer.
	void RenderTarget::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Resizes the attachments, but only when the size actually changed.
	void RenderTarget::Resize(uint32_t width, uint32_t height)
	{
		if (m_Width == width && m_Height == height)
			return;

		m_Width = width;
		m_Height = height;
		AllocateAttachments();
	}

	void RenderTarget::Resize(float width, float height)
	{
		Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	}

	void RenderTarget::Resize(const glm::vec2& size)
	{
		Resize(static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y));
	}

	// (Re)allocates the color texture and depth renderbuffer at the current size
	// and asserts the framebuffer is complete.
	void RenderTarget::AllocateAttachments()
	{
		TILES_ASSERT(m_Width > 0 && m_Height > 0, "Render target dimensions must be positive");

		glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		TILES_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Reads back the color attachment and encodes it to path, choosing the
	// encoder from the file extension.
	bool RenderTarget::SaveToFile(const std::string& path)
	{
		std::vector<uint8_t> pixels(static_cast<size_t>(m_Width) * m_Height * 4);

		Bind();
		glReadPixels(0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
		Unbind();

		// glReadPixels returns rows bottom-to-top; image encoders expect
		// top-to-bottom, so flip on write to avoid an upside-down image.
		stbi_flip_vertically_on_write(true);

		size_t dotPos = path.find_last_of('.');
		if (dotPos == std::string::npos)
		{
			TILES_ENGINE_ERROR("RenderTarget::SaveToFile: Path has no file extension: {}", path);
			return false;
		}

		std::string extension = path.substr(dotPos);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		int result = 0;
		if (extension == ".png")
			result = stbi_write_png(path.c_str(), m_Width, m_Height, 4, pixels.data(), m_Width * 4);
		else if (extension == ".jpg" || extension == ".jpeg")
			result = stbi_write_jpg(path.c_str(), m_Width, m_Height, 4, pixels.data(), 90);
		else if (extension == ".bmp")
			result = stbi_write_bmp(path.c_str(), m_Width, m_Height, 4, pixels.data());
		else if (extension == ".tga")
			result = stbi_write_tga(path.c_str(), m_Width, m_Height, 4, pixels.data());
		else
		{
			TILES_ENGINE_ERROR("RenderTarget::SaveToFile: Unsupported file format: {}", extension);
			return false;
		}

		if (result == 0)
		{
			TILES_ENGINE_ERROR("RenderTarget::SaveToFile: Failed to save render target to file: {}", path);
			return false;
		}

		TILES_ENGINE_INFO("Successfully saved render target to: {}", path);
		return true;
	}
}
