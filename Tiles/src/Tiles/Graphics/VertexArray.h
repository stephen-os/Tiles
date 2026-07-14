#pragma once

#include "Buffer.h"

#include <memory>

namespace Tiles
{
	// Vertex array object: binds a vertex buffer (wiring its layout to GL
	// attribute slots) and an optional index buffer. RAII wrapper owning a
	// single GL vertex-array handle.
	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		// Owns a GL vertex-array handle freed in the destructor; copying or moving
		// would risk a double free, and it is only ever held via shared_ptr, so
		// neither is permitted.
		VertexArray(const VertexArray&) = delete;
		VertexArray& operator=(const VertexArray&) = delete;
		VertexArray(VertexArray&&) = delete;
		VertexArray& operator=(VertexArray&&) = delete;

		// Binds this vertex array.
		void Bind() const;

		// Unbinds any vertex array.
		void Unbind() const;

		// Attaches a vertex buffer and wires its layout to attribute slots.
		void SetVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer);

		// Attaches the index buffer used for indexed draws.
		void SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer);

		// The attached vertex buffer, if any.
		[[nodiscard]] const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const { return m_VertexBuffer; }

		// The attached index buffer, if any.
		[[nodiscard]] const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

		// The GL vertex-array handle (0 if creation failed).
		[[nodiscard]] uint32_t GetID() const { return m_RendererID; }

		// Allocates a vertex array.
		[[nodiscard]] static std::shared_ptr<VertexArray> Create();

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_VertexBufferIndex = 0;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};
}
