#include "RendererState.h"

#include "Utils/FileReader.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Tiles
{
	void GridBatch::Init(const std::vector<uint32_t>& quadIndices)
	{
		VAO = VertexArray::Create();
		VBO = VertexBuffer::Create(MaxGrids * 4 * sizeof(GridVertex));
		VBO->SetLayout({
			{ BufferDataType::Float3, "a_Position" },
			{ BufferDataType::Float4, "a_Color" },
			{ BufferDataType::Float2, "a_TexCoord" },
			{ BufferDataType::Float,  "a_TexIndex" },
			{ BufferDataType::Float3, "a_GridPosition" },
			{ BufferDataType::Float2, "a_GridSize" },
			{ BufferDataType::Float,  "a_CellSize" },
			{ BufferDataType::Float4, "a_GridColor" },
			{ BufferDataType::Float,  "a_LineWidth" },
			{ BufferDataType::Float,  "a_ShowCheckerboard" },
			{ BufferDataType::Float4, "a_CheckerColor1" },
			{ BufferDataType::Float4, "a_CheckerColor2" }
			});
		VAO->SetVertexBuffer(VBO);

		IBO = IndexBuffer::Create(const_cast<uint32_t*>(quadIndices.data()), MaxIndices);
		VAO->SetIndexBuffer(IBO);
		Base = new GridVertex[MaxGrids * 4];

		{
			std::string vertexSource = ReadFile("res/shaders/Grid.vert");
			std::string fragmentSource = ReadFile("res/shaders/Grid.frag");
			Shader = ShaderProgram::Create(vertexSource, fragmentSource);
		}
	}

	void GridBatch::Shutdown()
	{
		VAO.reset();
		VBO.reset();
		IBO.reset();
		Shader.reset();

		delete[] Base;
		Base = nullptr;
		Ptr = nullptr;
	}

	void GridBatch::Append(RendererState& state)
	{
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), Position);
		glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 rotation = rotationZ * rotationY * rotationX;
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(Size, 1.0f));
		glm::mat4 transform = translation * rotation * scale;

		glm::vec2 texCoords[4] =
		{
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },
			{ 1.0f, 0.0f },
			{ 0.0f, 0.0f }
		};

		for (size_t i = 0; i < 4; i++)
		{
			Ptr->Position = transform * state.QuadVertexPositions[i];
			Ptr->Color = Color;
			Ptr->TexCoord = texCoords[i];
			Ptr->TexIndex = 0.0f;

			Ptr->GridPosition = Position;
			Ptr->GridSize = Size;
			Ptr->CellSize = CellSize;
			Ptr->GridColor = Color;
			Ptr->LineWidth = LineWidth;
			Ptr->ShowCheckerboard = ShowCheckerboard ? 1.0f : 0.0f;
			Ptr->CheckerColor1 = CheckerColor1;
			Ptr->CheckerColor2 = CheckerColor2;

			Ptr++;
		}

		IndexCount += 6;
		state.Stats.GridCount++;
	}

	void GridBatch::Reset()
	{
		IndexCount = 0;
		Ptr = Base;
	}

	bool GridBatch::Upload(RendererState& state)
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

	void GridBatch::Flush(RendererState& state)
	{
		if (IndexCount > 0)
		{
			Shader->Bind();
			Shader->SetUniformMat4("u_ViewProjection", state.ViewProjectionMatrix);

			VAO->Bind();
			RenderCommands::DrawElementsWithCount(VAO, PrimitiveType::Triangles, IndexCount);
			VAO->Unbind();
			Shader->Unbind();
			state.Stats.DrawCalls++;
		}

		IndexCount = 0;
	}
}
