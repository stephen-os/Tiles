#include "VertexArray.h"

#include <glad/gl.h>

namespace Tiles
{
	std::shared_ptr<VertexArray> VertexArray::Create()
	{
		return std::make_shared<VertexArray>();
	}

	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &m_RendererID);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	// Attaches a vertex buffer and wires each of its layout elements to the next
	// attribute index. Padding elements advance the stride but get no attribute,
	// and integer types use glVertexAttribIPointer to avoid float conversion.
	void VertexArray::SetVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer)
	{
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			if (element.IsPadding())
				continue;

			switch (element.Type)
			{
				case BufferDataType::Float:
				case BufferDataType::Float2:
				case BufferDataType::Float3:
				case BufferDataType::Float4:
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribPointer(m_VertexBufferIndex,
						element.GetComponentCount(),
						GL_FLOAT,
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)element.Offset);
					m_VertexBufferIndex++;
					break;
				}
				case BufferDataType::Int:
				case BufferDataType::Int2:
				case BufferDataType::Int3:
				case BufferDataType::Int4:
				case BufferDataType::Bool:
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribIPointer(m_VertexBufferIndex,
						element.GetComponentCount(),
						GL_INT,
						layout.GetStride(),
						(const void*)element.Offset);
					m_VertexBufferIndex++;
					break;
				}
			}
		}

		m_VertexBuffer = vertexBuffer;
	}

	void VertexArray::SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();
		m_IndexBuffer = indexBuffer;
	}
}
