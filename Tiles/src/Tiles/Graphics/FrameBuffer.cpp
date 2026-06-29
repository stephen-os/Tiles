#include "FrameBuffer.h"

#include "OpenGL/OpenGLFrameBuffer.h"

namespace Tiles
{
	std::shared_ptr<FrameBuffer> FrameBuffer::Create()
	{
		return std::make_shared<OpenGLFrameBuffer>();
	}
}
