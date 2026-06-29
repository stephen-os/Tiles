#include "VertexArray.h"
#include "OpenGL/OpenGLVertexArray.h"

namespace Tiles
{
	std::shared_ptr<VertexArray> VertexArray::Create()
	{
		// Tiles uses OpenGL directly
		return std::make_shared<OpenGLVertexArray>();
	}
}
