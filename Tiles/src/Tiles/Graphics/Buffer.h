#pragma once

#include "BufferLayout.h"

#include <cstdint>
#include <memory>

namespace Tiles
{
	enum class BufferUsage
	{
		Static = 0,     // Data uploaded once, rarely modified (GL_STATIC_DRAW)
		Dynamic,        // Data modified frequently (GL_DYNAMIC_DRAW)
		Stream          // Data modified every frame (GL_STREAM_DRAW)
	};

	// GPU vertex buffer; RAII wrapper owning a single GL buffer object.
	class VertexBuffer
	{
	public:
		// Allocates an uninitialized vertex buffer of size bytes.
		[[nodiscard]] static std::shared_ptr<VertexBuffer> Create(uint32_t size, BufferUsage usage = BufferUsage::Static);

		// Allocates a vertex buffer initialized with size bytes from data.
		[[nodiscard]] static std::shared_ptr<VertexBuffer> Create(const void* data, uint32_t size, BufferUsage usage = BufferUsage::Static);

		VertexBuffer(uint32_t size, BufferUsage usage = BufferUsage::Static);
		VertexBuffer(const void* data, uint32_t size, BufferUsage usage = BufferUsage::Static);
		~VertexBuffer();

		// Owns a GL buffer handle freed in the destructor. Copying would double
		// free it, so the type is move-only: a moved-from buffer keeps id 0 and
		// its destructor becomes a no-op.
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&& other) noexcept;
		VertexBuffer& operator=(VertexBuffer&& other) noexcept;

		// Binds this buffer to GL_ARRAY_BUFFER.
		void Bind() const;

		// Unbinds any array buffer.
		void Unbind() const;

		// Replaces the buffer contents, reallocating only to grow.
		void SetData(const void* data, uint32_t size);

		// The vertex attribute layout describing this buffer's contents.
		[[nodiscard]] const BufferLayout& GetLayout() const { return m_Layout; }

		// Sets the vertex attribute layout.
		void SetLayout(const BufferLayout& layout) { m_Layout = layout; }

		// The usage hint the buffer was created with.
		[[nodiscard]] BufferUsage GetUsage() const { return m_Usage; }

		// The current buffer size in bytes.
		[[nodiscard]] uint32_t GetSize() const { return m_Size; }

		// The GL buffer handle.
		[[nodiscard]] uint32_t GetID() const { return m_BufferID; }

	private:
		uint32_t m_BufferID;
		uint32_t m_Size;
		BufferUsage m_Usage;
		BufferLayout m_Layout;
	};

	// GPU element/index buffer; RAII wrapper owning a single GL buffer object.
	class IndexBuffer
	{
	public:
		// Allocates an index buffer initialized with count 32-bit indices.
		[[nodiscard]] static std::shared_ptr<IndexBuffer> Create(uint32_t* data, uint32_t count, BufferUsage usage = BufferUsage::Static);

		IndexBuffer(uint32_t* data, uint32_t count, BufferUsage usage = BufferUsage::Static);
		~IndexBuffer();

		// Move-only for the same GL-ownership reason as VertexBuffer.
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer& operator=(const IndexBuffer&) = delete;
		IndexBuffer(IndexBuffer&& other) noexcept;
		IndexBuffer& operator=(IndexBuffer&& other) noexcept;

		// Binds this buffer to GL_ELEMENT_ARRAY_BUFFER.
		void Bind() const;

		// Unbinds any element array buffer.
		void Unbind() const;

		// Replaces the index contents, reallocating only to grow.
		void SetData(uint32_t* data, uint32_t count);

		// The number of indices.
		[[nodiscard]] uint32_t GetCount() const { return m_Count; }

		// The usage hint the buffer was created with.
		[[nodiscard]] BufferUsage GetUsage() const { return m_Usage; }

		// The GL buffer handle.
		[[nodiscard]] uint32_t GetID() const { return m_BufferID; }

	private:
		uint32_t m_BufferID;
		uint32_t m_Count;
		BufferUsage m_Usage;
	};

	// GPU uniform buffer (UBO) for sharing uniform blocks across shaders;
	// RAII wrapper owning a single GL buffer object.
	class UniformBuffer
	{
	public:
		// Allocates an uninitialized uniform buffer of size bytes.
		[[nodiscard]] static std::shared_ptr<UniformBuffer> Create(uint32_t size, BufferUsage usage = BufferUsage::Dynamic);

		// Allocates a uniform buffer initialized with size bytes from data.
		[[nodiscard]] static std::shared_ptr<UniformBuffer> Create(const void* data, uint32_t size, BufferUsage usage = BufferUsage::Dynamic);

		UniformBuffer(uint32_t size, BufferUsage usage = BufferUsage::Dynamic);
		UniformBuffer(const void* data, uint32_t size, BufferUsage usage = BufferUsage::Dynamic);
		~UniformBuffer();

		// Move-only for the same GL-ownership reason as VertexBuffer.
		UniformBuffer(const UniformBuffer&) = delete;
		UniformBuffer& operator=(const UniformBuffer&) = delete;
		UniformBuffer(UniformBuffer&& other) noexcept;
		UniformBuffer& operator=(UniformBuffer&& other) noexcept;

		// Binds the buffer to an indexed uniform binding point (glBindBufferBase)
		// and remembers it so Unbind() can release the same point.
		void Bind(uint32_t bindingPoint) const;

		// Releases the binding point the buffer was last bound to.
		void Unbind() const;

		// Replaces the buffer contents, reallocating only to grow.
		void SetData(const void* data, uint32_t size);

		// Updates size bytes at offset without reallocating.
		void SetSubData(const void* data, uint32_t size, uint32_t offset);

		// The current buffer size in bytes.
		[[nodiscard]] uint32_t GetSize() const { return m_Size; }

		// The usage hint the buffer was created with.
		[[nodiscard]] BufferUsage GetUsage() const { return m_Usage; }

		// The GL buffer handle.
		[[nodiscard]] uint32_t GetID() const { return m_BufferID; }

	private:
		uint32_t m_BufferID;
		uint32_t m_Size;
		BufferUsage m_Usage;
		mutable uint32_t m_CurrentBindingPoint = 0;
	};
}
