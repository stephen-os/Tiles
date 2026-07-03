#include "RendererState.h"

#include "Utils/FileReader.h"

namespace Tiles
{
	void LineBatch::Init()
	{
		VAO = VertexArray::Create();
		VBO = VertexBuffer::Create(MaxVertices * sizeof(LineVertex));
		VBO->SetLayout({
			{ BufferDataType::Float3, "a_Position" },
			{ BufferDataType::Float4, "a_Color" }
			});
		VAO->SetVertexBuffer(VBO);
		Base = new LineVertex[MaxVertices];

		{
			std::string vertexSource = ReadFile("res/shaders/Line.vert");
			std::string fragmentSource = ReadFile("res/shaders/Line.frag");
			Shader = ShaderProgram::Create(vertexSource, fragmentSource);
		}
	}

	void LineBatch::Shutdown()
	{
		VAO.reset();
		VBO.reset();
		Shader.reset();

		delete[] Base;
		Base = nullptr;
		Ptr = nullptr;
	}

	void LineBatch::Append(RendererState& state, const LineParams& params)
	{
		Ptr->Position = params.Start;
		Ptr->Color = params.Color;
		Ptr++;
		VertexCount++;

		Ptr->Position = params.End;
		Ptr->Color = params.Color;
		Ptr++;
		VertexCount++;

		state.Stats.LineCount++;
	}

	void LineBatch::Reset()
	{
		VertexCount = 0;
		Ptr = Base;
	}

	bool LineBatch::Upload(RendererState& state)
	{
		uint32_t size = CalculateBufferSize(Ptr, Base);
		if (size > 0)
		{
			state.Stats.DataSize += size;
			VBO->SetData(Base, size);
			return true;
		}
		return false;
	}

	void LineBatch::Flush(RendererState& state)
	{
		if (VertexCount > 0)
		{
			Shader->Bind();
			SetSharedUniforms(Shader, state);

			VAO->Bind();
			RenderCommands::SetLineWidth(Width);
			RenderCommands::DrawLines(VAO, VertexCount);
			VAO->Unbind();
			Shader->Unbind();
			state.Stats.DrawCalls++;
		}

		VertexCount = 0;
	}
}
