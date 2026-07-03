#include "GridRenderer.h"

#include "Graphics/RenderCommands.h"
#include "Utils/FileReader.h"

#include <glm/glm.hpp>

namespace Tiles::Editor
{
	GridRenderer::GridRenderer()
	{
		// Fullscreen quad in clip space, drawn as a triangle strip.
		float vertices[] =
		{
			-1.0f, -1.0f,
			 1.0f, -1.0f,
			-1.0f,  1.0f,
			 1.0f,  1.0f,
		};

		m_VAO = Tiles::VertexArray::Create();
		m_VBO = Tiles::VertexBuffer::Create(sizeof(vertices));
		m_VBO->SetLayout({ { Tiles::BufferDataType::Float2, "a_NDC" } });
		m_VAO->SetVertexBuffer(m_VBO);
		m_VBO->SetData(vertices, sizeof(vertices));

		std::string vertexSource = Tiles::ReadFile("res/shaders/Grid.vert");
		std::string fragmentSource = Tiles::ReadFile("res/shaders/Grid.frag");
		m_Shader = Tiles::ShaderProgram::Create(vertexSource, fragmentSource);
	}

	void GridRenderer::Draw(const GridParams& params)
	{
		m_Shader->Bind();
		m_Shader->SetUniformMat4("u_InvViewProjection", glm::inverse(params.ViewProjection));
		m_Shader->SetUniformFloat("u_CellSize", params.CellSize);
		m_Shader->SetUniformFloat("u_MajorEvery", params.MajorEvery);
		m_Shader->SetUniformVec4("u_LineColor", params.LineColor);
		m_Shader->SetUniformVec4("u_MajorLineColor", params.MajorLineColor);
		m_Shader->SetUniformVec4("u_BackgroundColor", params.BackgroundColor);

		// Background pass: cover the viewport but leave depth untouched so the
		// tiles and lines drawn afterward always sit on top.
		Tiles::RenderCommands::SetDepthMask(false);
		Tiles::RenderCommands::DrawTriangleStrip(m_VAO, 4);
		Tiles::RenderCommands::SetDepthMask(true);

		m_Shader->Unbind();
	}
}
