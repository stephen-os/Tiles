#include "RendererState.h"

#include "Utils/FileReader.h"

namespace Tiles
{
	void TriangleBatch::Init()
	{
		VAO = VertexArray::Create();
		VBO = VertexBuffer::Create(MaxTriangles * 3 * sizeof(TriangleVertex));
		VBO->SetLayout({
			{ BufferDataType::Float3, "a_Position" },
			{ BufferDataType::Float4, "a_Color" },
			{ BufferDataType::Float2, "a_TexCoord" },
			{ BufferDataType::Float,  "a_TexIndex" }
			});
		VAO->SetVertexBuffer(VBO);
		Base = new TriangleVertex[MaxTriangles * 3];

		{
			std::string vertexSource = ReadFile("res/shaders/Triangle.vert");
			std::string fragmentSource = ReadFile("res/shaders/Triangle.frag");
			Shader = ShaderProgram::Create(vertexSource, fragmentSource);
		}
	}

	void TriangleBatch::Shutdown()
	{
		VAO.reset();
		VBO.reset();
		Shader.reset();

		delete[] Base;
		Base = nullptr;
		Ptr = nullptr;
	}

	void TriangleBatch::Append(RendererState& state)
	{
		float texIndex = state.Textures.ComputeTextureIndex(Texture);

		Ptr->Position = Point1;
		Ptr->Color = Color;
		Ptr->TexCoord = { 0.0f, 1.0f };
		Ptr->TexIndex = texIndex;
		Ptr++;

		Ptr->Position = Point2;
		Ptr->Color = Color;
		Ptr->TexCoord = { 1.0f, 1.0f };
		Ptr->TexIndex = texIndex;
		Ptr++;

		Ptr->Position = Point3;
		Ptr->Color = Color;
		Ptr->TexCoord = { 0.5f, 0.0f };
		Ptr->TexIndex = texIndex;
		Ptr++;

		VertexCount += 3;
		state.Stats.TriangleCount++;
	}

	void TriangleBatch::Reset()
	{
		VertexCount = 0;
		Ptr = Base;
	}

	bool TriangleBatch::Upload(RendererState& state)
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

	void TriangleBatch::Flush(RendererState& state)
	{
		if (VertexCount > 0)
		{
			Shader->Bind();
			SetSharedUniforms(Shader, state);

			VAO->Bind();
			RenderCommands::DrawArrays(VAO, PrimitiveType::Triangles, VertexCount);
			VAO->Unbind();
			Shader->Unbind();
			state.Stats.DrawCalls++;
		}

		VertexCount = 0;
	}
}
