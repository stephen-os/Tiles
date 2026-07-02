#pragma once

#include "../VertexArray.h"

namespace Tiles
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray() override;

		// Owns a GL vertex-array handle freed in the destructor; copying or
		// moving would risk a double free, and it is only ever held via
		// shared_ptr, so neither is permitted.
		OpenGLVertexArray(const OpenGLVertexArray&) = delete;
		OpenGLVertexArray& operator=(const OpenGLVertexArray&) = delete;
		OpenGLVertexArray(OpenGLVertexArray&&) = delete;
		OpenGLVertexArray& operator=(OpenGLVertexArray&&) = delete;

		void Bind() const override;
		void Unbind() const override;

		void SetVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer) override;
		void SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer) override;

		std::shared_ptr<VertexBuffer> GetVertexBuffer() override { return m_VertexBuffer; }
		std::shared_ptr<IndexBuffer> GetIndexBuffer() override { return m_IndexBuffer; }

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_VertexBufferIndex = 0;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};
}
