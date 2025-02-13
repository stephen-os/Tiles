#include "Quad.h"
Quad::Quad()
{
    std::vector<float> vertices =
    {
        // Positions        Color             UV
         0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };

    std::vector<unsigned int> indices =
    {
        0, 1, 2,
        2, 3, 0
    };

    // Create Vertex Buffer
    Lumina::Ref<Lumina::VertexBuffer> vertexBuffer = std::make_shared<Lumina::VertexBuffer>(
        vertices.data(),
        static_cast<uint32_t>(vertices.size() * sizeof(float))
    );

    // Define Buffer Layout
    Lumina::BufferLayout layout = {
        { Lumina::BufferDataType::Float3, "a_Position" },
        { Lumina::BufferDataType::Float3, "a_Color" },
        { Lumina::BufferDataType::Float2, "a_TexCoord" },
    };
    vertexBuffer->SetLayout(layout);

    // Create Index Buffer
    Lumina::Ref<Lumina::IndexBuffer> indexBuffer = std::make_shared<Lumina::IndexBuffer>(
        indices.data(),
        static_cast<uint32_t>(indices.size())
    );

    // Create Vertex Array and Set Buffers
    m_VertexArray = std::make_shared<Lumina::VertexArray>();
    m_VertexArray->AddVertexBuffer(vertexBuffer);
    m_VertexArray->SetIndexBuffer(indexBuffer);
}