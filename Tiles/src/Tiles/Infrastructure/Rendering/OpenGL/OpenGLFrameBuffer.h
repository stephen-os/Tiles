#pragma once

#include <glad/glad.h>

#include <cstdint>

#include "Graphics/FrameBuffer.h"

namespace Tiles
{
    class OpenGLFrameBuffer : public FrameBuffer
    {
    public:
        OpenGLFrameBuffer();
        ~OpenGLFrameBuffer();

        void Bind() const override;
        void Unbind() const override;
        void Resize(uint32_t width, uint32_t height) override;

        uint32_t GetID() const override { return m_BufferID; }
        uint32_t GetColorAttachment() const override { return m_ColorAttachment; }

        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }

        void ReadPixels(int x, int y, uint32_t width, uint32_t height, void* data) const override;
    private:
        uint32_t m_BufferID = 0;
        uint32_t m_ColorAttachment = 0;
        uint32_t m_DepthAttachment = 0;

        uint32_t m_Width = 900;
        uint32_t m_Height = 900;
    };
}
