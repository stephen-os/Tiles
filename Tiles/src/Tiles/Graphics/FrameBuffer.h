#pragma once

#include "../Core/Aliases.h"

namespace Tiles
{
	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetID() const = 0;
		virtual uint32_t GetColorAttachment() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void ReadPixels(int x, int y, uint32_t width, uint32_t height, void* data) const = 0;

		static Shared<FrameBuffer> Create();
	};
}
