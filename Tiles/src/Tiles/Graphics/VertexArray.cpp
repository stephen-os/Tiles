#include "VertexArray.h"
#include "OpenGL/OpenGLVertexArray.h"

namespace Tiles
{
	Shared<VertexArray> VertexArray::Create()
	{
		// Tiles uses OpenGL directly
		return MakeShared<OpenGLVertexArray>();
	}
}
