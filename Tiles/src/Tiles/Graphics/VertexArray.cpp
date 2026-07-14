#include "VertexArray.h"

#include "Core/Logger.h"

#include <glad/gl.h>

namespace Tiles
{
	// Allocates a vertex array.
	std::shared_ptr<VertexArray> VertexArray::Create()
	{
		return std::make_shared<VertexArray>();
	}

	// Creates the GL vertex-array handle.
	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &m_RendererID);
		if (m_RendererID == 0)
			TILES_ENGINE_ERROR("VertexArray: failed to create the GL vertex array.");
	}

	// Deletes the GL vertex-array handle.
	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	// Binds this vertex array.
	void VertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	// Unbinds any vertex array.
	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	// Attaches a vertex buffer and wires each of its layout elements to the next
	// attribute index. Padding elements advance the stride but get no attribute,
	// integer types use glVertexAttribIPointer to avoid float conversion, and
	// matrix types occupy one attribute slot per column.
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
						reinterpret_cast<const void*>(element.Offset));
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
						reinterpret_cast<const void*>(element.Offset));
					m_VertexBufferIndex++;
					break;
				}
				case BufferDataType::Mat3:
				case BufferDataType::Mat4:
				{
					// A matrix attribute takes one slot per column (mat3 = 3 x vec3,
					// mat4 = 4 x vec4), each advanced by a column's worth of floats.
					const uint32_t columns = element.GetComponentCount();
					for (uint32_t col = 0; col < columns; ++col)
					{
						glEnableVertexAttribArray(m_VertexBufferIndex);
						glVertexAttribPointer(m_VertexBufferIndex,
							columns,
							GL_FLOAT,
							element.Normalized ? GL_TRUE : GL_FALSE,
							layout.GetStride(),
							reinterpret_cast<const void*>(element.Offset + col * columns * sizeof(float)));
						m_VertexBufferIndex++;
					}
					break;
				}
			}
		}

		m_VertexBuffer = vertexBuffer;
	}

	// Attaches the index buffer used for indexed draws.
	void VertexArray::SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();
		m_IndexBuffer = indexBuffer;
	}
}
