#pragma once
#include <memory>

#include <cstdint>


#include "BufferLayout.h"

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
        static std::shared_ptr<VertexBuffer> Create(uint32_t size, BufferUsage usage = BufferUsage::Static);
        static std::shared_ptr<VertexBuffer> Create(const void* data, uint32_t size, BufferUsage usage = BufferUsage::Static);

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

        void Bind() const;
        void Unbind() const;
        void SetData(const void* data, uint32_t size);

        const BufferLayout& GetLayout() const { return m_Layout; }
        void SetLayout(const BufferLayout& layout) { m_Layout = layout; }

        BufferUsage GetUsage() const { return m_Usage; }
        uint32_t GetSize() const { return m_Size; }

		uint32_t GetID() const { return m_BufferID; }

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
        static std::shared_ptr<IndexBuffer> Create(uint32_t* data, uint32_t count, BufferUsage usage = BufferUsage::Static);

        IndexBuffer(uint32_t* data, uint32_t count, BufferUsage usage = BufferUsage::Static);
        virtual ~IndexBuffer();

        // Move-only for the same GL-ownership reason as VertexBuffer.
        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;
        IndexBuffer(IndexBuffer&& other) noexcept;
        IndexBuffer& operator=(IndexBuffer&& other) noexcept;

        void Bind() const;
        void Unbind() const;
        void SetData(uint32_t* data, uint32_t count);

        uint32_t GetCount() const { return m_Count; }
        BufferUsage GetUsage() const { return m_Usage; }

		uint32_t GetID() const { return m_BufferID; }

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
        static std::shared_ptr<UniformBuffer> Create(uint32_t size, BufferUsage usage = BufferUsage::Dynamic);
        static std::shared_ptr<UniformBuffer> Create(const void* data, uint32_t size, BufferUsage usage = BufferUsage::Dynamic);

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
        void Unbind() const;

        void SetData(const void* data, uint32_t size);
        void SetSubData(const void* data, uint32_t size, uint32_t offset);

        uint32_t GetSize() const { return m_Size; }
        BufferUsage GetUsage() const { return m_Usage; }
        uint32_t GetID() const { return m_BufferID; }

    private:
        uint32_t m_BufferID;
        uint32_t m_Size;
        BufferUsage m_Usage;
        mutable uint32_t m_CurrentBindingPoint = 0;
    };
}
