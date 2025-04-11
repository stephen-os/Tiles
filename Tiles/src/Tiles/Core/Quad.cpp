#include "Quad.h"

#include <Lumina/Renderer/Buffer.h>
#include <Lumina/Utils/FileReader.h>

#include "../Core/Base.h"

namespace Tiles
{
	Quad::Quad()
	{
        float vertices[] =
        {
            // Positions    
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f
        };

        uint32_t indices[] =
        {
            0, 1, 2,
            2, 3, 0
        };

        m_VertexArray = Lumina::VertexArray::Create();

        auto vertexBuffer = Lumina::VertexBuffer::Create(vertices, sizeof(vertices));
        auto indexBuffer = Lumina::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

        vertexBuffer->SetLayout({ { Lumina::BufferDataType::Float3, "a_Position" } });

        m_VertexArray->AddVertexBuffer(vertexBuffer);
        m_VertexArray->SetIndexBuffer(indexBuffer);

        const std::string vertexSrc = Lumina::ReadFile("res/shaders/GridShader.vert");
        const std::string fragmentSrc = Lumina::ReadFile("res/shaders/GridShader.frag");
        m_Shader = Lumina::ShaderProgram::Create(vertexSrc, fragmentSrc);
	}

    void Quad::SetUniforms(const Camera& camera, const Shared<Layers>& layers) const
    {
        m_Shader->SetUniformMat4("u_ViewProjection", camera.GetViewMatrix());
        m_Shader->SetUniformVec2("u_GridSize", { layers->GetWidth() * 4.0f, layers->GetHeight() * 4.0f }); // x4 for 4x4 checkerboard pattern
    }

    void Quad::Bind() const
	{
		m_Shader->Bind();
		m_VertexArray->Bind();
	}

    void Quad::Unbind() const
    {
        m_Shader->Unbind();
		m_VertexArray->Unbind();
    }
}