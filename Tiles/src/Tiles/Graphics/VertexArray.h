#pragma once

#include "Buffer.h"

#include "../Core/Aliases.h"

namespace Tiles
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetVertexBuffer(Shared<VertexBuffer> vertexBuffer) = 0;
		virtual void SetIndexBuffer(Shared<IndexBuffer> indexBuffer) = 0;

		virtual Shared<VertexBuffer> GetVertexBuffer() = 0;
		virtual Shared<IndexBuffer> GetIndexBuffer() = 0;

		static Shared<VertexArray> Create();
	};
}
