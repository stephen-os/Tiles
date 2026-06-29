#pragma once

#include "Buffer.h"

#include <memory>

namespace Tiles
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer) = 0;
		virtual void SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer) = 0;

		virtual std::shared_ptr<VertexBuffer> GetVertexBuffer() = 0;
		virtual std::shared_ptr<IndexBuffer> GetIndexBuffer() = 0;

		static std::shared_ptr<VertexArray> Create();
	};
}
