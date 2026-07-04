#include "RendererState.h"

#include "Utils/FileReader.h"

#include <glm/glm.hpp>

namespace Tiles
{
	void GridPass::Init()
	{
		// Fullscreen quad in clip space, drawn as a triangle strip.
		float vertices[] =
		{
			-1.0f, -1.0f,
			 1.0f, -1.0f,
			-1.0f,  1.0f,
			 1.0f,  1.0f,
		};

		m_VAO = VertexArray::Create();
		m_VBO = VertexBuffer::Create(sizeof(vertices));
		m_VBO->SetLayout({ { BufferDataType::Float2, "a_NDC" } });
		m_VAO->SetVertexBuffer(m_VBO);
		m_VBO->SetData(vertices, sizeof(vertices));

		std::string vertexSource = ReadFile("res/shaders/Grid.vert");
		std::string fragmentSource = ReadFile("res/shaders/Grid.frag");
		m_Shader = ShaderProgram::Create(vertexSource, fragmentSource);
	}

	void GridPass::Shutdown()
	{
		m_VAO.reset();
		m_VBO.reset();
		m_Shader.reset();
	}

	void GridPass::Draw(RendererState& state, const GridParams& params)
	{
		m_Shader->Bind();
		m_Shader->SetUniformMat4("u_InvViewProjection", glm::inverse(state.ViewProjectionMatrix));
		m_Shader->SetUniformFloat("u_CellSize", params.CellSize);
		m_Shader->SetUniformFloat("u_MajorEvery", params.MajorEvery);
		m_Shader->SetUniformVec2("u_GridOffset", params.Offset);
		m_Shader->SetUniformVec4("u_LineColor", params.LineColor);
		m_Shader->SetUniformVec4("u_MajorLineColor", params.MajorLineColor);
		m_Shader->SetUniformVec4("u_BackgroundColor", params.BackgroundColor);

		// Cover the viewport but leave depth untouched, so the tiles and lines
		// drawn afterward always sit on top.
		RenderCommands::SetDepthMask(false);
		RenderCommands::DrawTriangleStrip(m_VAO, 4);
		RenderCommands::SetDepthMask(true);

		m_Shader->Unbind();

		state.Stats.DrawCalls++;
	}
}
