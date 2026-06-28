#include "FrameBuffer.h"

#include "OpenGL/OpenGLFrameBuffer.h"

namespace Tiles
{
	Shared<FrameBuffer> FrameBuffer::Create()
	{
		return MakeShared<OpenGLFrameBuffer>();
	}
}
