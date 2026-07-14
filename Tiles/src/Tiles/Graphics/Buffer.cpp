#include "Buffer.h"

#include "Core/Assert.h"

#include <glad/gl.h>

#include <utility>

namespace Tiles
{
	namespace
	{
		// Maps a BufferUsage to its GL_*_DRAW enum.
		uint32_t UsageToEnum(BufferUsage usage)
		{
			switch (usage)
			{
			case BufferUsage::Static:   return GL_STATIC_DRAW;
			case BufferUsage::Dynamic:  return GL_DYNAMIC_DRAW;
			case BufferUsage::Stream:   return GL_STREAM_DRAW;
			}

			TILES_ASSERT(false, "Unknown BufferUsage enum value!");
			return GL_STATIC_DRAW;
		}
	}

	// --- Vertex buffer ------------------------------------------------------

	// Allocates an uninitialized vertex buffer of size bytes.
	std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size, BufferUsage usage)
	{
		return std::make_shared<VertexBuffer>(size, usage);
	}

	// Allocates a vertex buffer initialized with size bytes from data.
	std::shared_ptr<VertexBuffer> VertexBuffer::Create(const void* data, uint32_t size, BufferUsage usage)
	{
		return std::make_shared<VertexBuffer>(data, size, usage);
	}

	// Creates the GL buffer and allocates empty storage.
	VertexBuffer::VertexBuffer(uint32_t size, BufferUsage usage) : m_Size(size), m_Usage(usage)
	{
		glCreateBuffers(1, &m_BufferID);
		if (m_BufferID == 0)
			TILES_ENGINE_ERROR("VertexBuffer: failed to create the GL buffer.");

		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, UsageToEnum(m_Usage));
	}

	// Creates the GL buffer and uploads the initial vertices.
	VertexBuffer::VertexBuffer(const void* vertices, uint32_t size, BufferUsage usage) : m_Size(size), m_Usage(usage)
	{
		glCreateBuffers(1, &m_BufferID);
		if (m_BufferID == 0)
			TILES_ENGINE_ERROR("VertexBuffer: failed to create the GL buffer.");

		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, UsageToEnum(m_Usage));
	}

	// Deletes the GL buffer (a no-op for a moved-from id of 0).
	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	// Steals other's handle, leaving it id 0.
	VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
		: m_BufferID(other.m_BufferID), m_Size(other.m_Size), m_Usage(other.m_Usage), m_Layout(std::move(other.m_Layout))
	{
		other.m_BufferID = 0;
	}

	// Frees this handle, then steals other's.
	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
	{
		if (this != &other)
		{
			glDeleteBuffers(1, &m_BufferID);
			m_BufferID = other.m_BufferID;
			m_Size = other.m_Size;
			m_Usage = other.m_Usage;
			m_Layout = std::move(other.m_Layout);
			other.m_BufferID = 0;
		}
		return *this;
	}

	// Binds this buffer to GL_ARRAY_BUFFER.
	void VertexBuffer::Bind() const
	{
		TILES_ASSERT(m_BufferID != 0, "Trying to bind an invalid vertex buffer!");
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}

	// Unbinds any array buffer.
	void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// Replaces the buffer contents, reusing storage when the update fits.
	void VertexBuffer::SetData(const void* data, uint32_t size)
	{
		TILES_ASSERT(data != nullptr, "VertexBuffer::SetData called with null data!");
		TILES_ASSERT(size > 0, "VertexBuffer::SetData called with zero size!");

		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);

		// Reuse existing storage when the update fits; only reallocate to grow.
		if (size <= m_Size)
			glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
		else
			glBufferData(GL_ARRAY_BUFFER, size, data, UsageToEnum(m_Usage));

		m_Size = size;
	}

	// --- Index buffer -------------------------------------------------------

	// Allocates an index buffer initialized with count 32-bit indices.
	std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* data, uint32_t count, BufferUsage usage)
	{
		return std::make_shared<IndexBuffer>(data, count, usage);
	}

	// Creates the GL buffer and uploads the initial indices.
	IndexBuffer::IndexBuffer(uint32_t* data, uint32_t count, BufferUsage usage) : m_Count(count), m_Usage(usage)
	{
		TILES_ASSERT(data != nullptr, "Null index data passed to IndexBuffer!");
		TILES_ASSERT(count > 0, "Index buffer count is zero!");

		glCreateBuffers(1, &m_BufferID);
		if (m_BufferID == 0)
			TILES_ENGINE_ERROR("IndexBuffer: failed to create the GL buffer.");

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, UsageToEnum(m_Usage));
	}

	// Deletes the GL buffer (a no-op for a moved-from id of 0).
	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	// Steals other's handle, leaving it id 0.
	IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
		: m_BufferID(other.m_BufferID), m_Count(other.m_Count), m_Usage(other.m_Usage)
	{
		other.m_BufferID = 0;
	}

	// Frees this handle, then steals other's.
	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept
	{
		if (this != &other)
		{
			glDeleteBuffers(1, &m_BufferID);
			m_BufferID = other.m_BufferID;
			m_Count = other.m_Count;
			m_Usage = other.m_Usage;
			other.m_BufferID = 0;
		}
		return *this;
	}

	// Binds this buffer to GL_ELEMENT_ARRAY_BUFFER.
	void IndexBuffer::Bind() const
	{
		TILES_ASSERT(m_BufferID != 0, "Trying to bind an invalid index buffer!");
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}

	// Unbinds any element array buffer.
	void IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// Replaces the index contents, reusing storage when the update fits.
	void IndexBuffer::SetData(uint32_t* data, uint32_t count)
	{
		TILES_ASSERT(data != nullptr, "IndexBuffer::SetData called with null data!");
		TILES_ASSERT(count > 0, "IndexBuffer::SetData called with zero count!");

		size_t newSize = static_cast<size_t>(count) * sizeof(uint32_t);
		size_t currentSize = static_cast<size_t>(m_Count) * sizeof(uint32_t);

		m_Count = count;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);

		// Reuse existing storage when the update fits; only reallocate to grow.
		if (newSize <= currentSize)
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, newSize, data);
		else
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, newSize, data, UsageToEnum(m_Usage));
	}

	// --- Uniform buffer -----------------------------------------------------

	// Allocates an uninitialized uniform buffer of size bytes.
	std::shared_ptr<UniformBuffer> UniformBuffer::Create(uint32_t size, BufferUsage usage)
	{
		return std::make_shared<UniformBuffer>(size, usage);
	}

	// Allocates a uniform buffer initialized with size bytes from data.
	std::shared_ptr<UniformBuffer> UniformBuffer::Create(const void* data, uint32_t size, BufferUsage usage)
	{
		return std::make_shared<UniformBuffer>(data, size, usage);
	}

	// Creates the GL buffer and allocates empty storage.
	UniformBuffer::UniformBuffer(uint32_t size, BufferUsage usage) : m_Size(size), m_Usage(usage)
	{
		glCreateBuffers(1, &m_BufferID);
		if (m_BufferID == 0)
			TILES_ENGINE_ERROR("UniformBuffer: failed to create the GL buffer.");

		glBindBuffer(GL_UNIFORM_BUFFER, m_BufferID);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, UsageToEnum(m_Usage));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	// Creates the GL buffer and uploads the initial block.
	UniformBuffer::UniformBuffer(const void* data, uint32_t size, BufferUsage usage) : m_Size(size), m_Usage(usage)
	{
		TILES_ASSERT(data != nullptr, "Null data passed to UniformBuffer constructor!");
		TILES_ASSERT(size > 0, "Uniform buffer size is zero!");

		glCreateBuffers(1, &m_BufferID);
		if (m_BufferID == 0)
			TILES_ENGINE_ERROR("UniformBuffer: failed to create the GL buffer.");

		glBindBuffer(GL_UNIFORM_BUFFER, m_BufferID);
		glBufferData(GL_UNIFORM_BUFFER, size, data, UsageToEnum(m_Usage));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	// Deletes the GL buffer (a no-op for a moved-from id of 0).
	UniformBuffer::~UniformBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	// Steals other's handle, leaving it id 0.
	UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept
		: m_BufferID(other.m_BufferID), m_Size(other.m_Size), m_Usage(other.m_Usage), m_CurrentBindingPoint(other.m_CurrentBindingPoint)
	{
		other.m_BufferID = 0;
	}

	// Frees this handle, then steals other's.
	UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept
	{
		if (this != &other)
		{
			glDeleteBuffers(1, &m_BufferID);
			m_BufferID = other.m_BufferID;
			m_Size = other.m_Size;
			m_Usage = other.m_Usage;
			m_CurrentBindingPoint = other.m_CurrentBindingPoint;
			other.m_BufferID = 0;
		}
		return *this;
	}

	// Binds the buffer to an indexed uniform binding point and remembers it.
	void UniformBuffer::Bind(uint32_t bindingPoint) const
	{
		TILES_ASSERT(m_BufferID != 0, "Trying to bind an invalid uniform buffer!");
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_BufferID);
		m_CurrentBindingPoint = bindingPoint;
	}

	// Releases the binding point the buffer was last bound to.
	void UniformBuffer::Unbind() const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, m_CurrentBindingPoint, 0);
	}

	// Replaces the buffer contents, reusing storage when the update fits.
	void UniformBuffer::SetData(const void* data, uint32_t size)
	{
		TILES_ASSERT(data != nullptr, "UniformBuffer::SetData called with null data!");
		TILES_ASSERT(size > 0, "UniformBuffer::SetData called with zero size!");

		glBindBuffer(GL_UNIFORM_BUFFER, m_BufferID);

		// Reuse existing storage when the update fits; only reallocate to grow.
		if (size <= m_Size)
			glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
		else
			glBufferData(GL_UNIFORM_BUFFER, size, data, UsageToEnum(m_Usage));

		m_Size = size;
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	// Updates size bytes at offset without reallocating.
	void UniformBuffer::SetSubData(const void* data, uint32_t size, uint32_t offset)
	{
		TILES_ASSERT(data != nullptr, "UniformBuffer::SetSubData called with null data!");
		TILES_ASSERT(size > 0, "UniformBuffer::SetSubData called with zero size!");
		TILES_ASSERT(offset + size <= m_Size, "UniformBuffer::SetSubData: offset + size exceeds buffer size!");

		glBindBuffer(GL_UNIFORM_BUFFER, m_BufferID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}
