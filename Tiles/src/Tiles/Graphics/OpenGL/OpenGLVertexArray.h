#pragma once

#include "../VertexArray.h"

namespace Tiles
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray() override;

		void Bind() const override;
		void Unbind() const override;

		void SetVertexBuffer(Shared<VertexBuffer> vertexBuffer) override;
		void SetIndexBuffer(Shared<IndexBuffer> indexBuffer) override;

		Shared<VertexBuffer> GetVertexBuffer() override { return m_VertexBuffer; }
		Shared<IndexBuffer> GetIndexBuffer() override { return m_IndexBuffer; }

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_VertexBufferIndex = 0;
		Shared<VertexBuffer> m_VertexBuffer;
		Shared<IndexBuffer> m_IndexBuffer;
	};
}
