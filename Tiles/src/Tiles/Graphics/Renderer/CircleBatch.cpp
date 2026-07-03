#include "RendererState.h"
#include "Core/Assert.h"

#include "Utils/FileReader.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Tiles
{
	void CircleBatch::Init(const std::vector<uint32_t>& quadIndices)
	{
		VAO = VertexArray::Create();
		VBO = VertexBuffer::Create(MaxVertices * sizeof(CircleVertex));
		VBO->SetLayout({
			{ BufferDataType::Float3, "a_WorldPosition" },
			{ BufferDataType::Float3, "a_LocalPosition" },
			{ BufferDataType::Float4, "a_Color" },
			{ BufferDataType::Float2, "a_TexCoord" },
			{ BufferDataType::Float,  "a_TexIndex" },
			{ BufferDataType::Float,  "a_Thickness" },
			{ BufferDataType::Float,  "a_Fade" }
			});
		VAO->SetVertexBuffer(VBO);

		IBO = IndexBuffer::Create(const_cast<uint32_t*>(quadIndices.data()), MaxIndices);
		VAO->SetIndexBuffer(IBO);
		Base = new CircleVertex[MaxVertices];

		{
			std::string vertexSource = ReadFile("res/shaders/Circle.vert");
			std::string fragmentSource = ReadFile("res/shaders/Circle.frag");
			Shader = ShaderProgram::Create(vertexSource, fragmentSource);
		}
	}

	void CircleBatch::Shutdown()
	{
		VAO.reset();
		VBO.reset();
		IBO.reset();
		Shader.reset();

		delete[] Base;
		Base = nullptr;
		Ptr = nullptr;
	}

	void CircleBatch::Append(RendererState& state, const CircleParams& params)
	{
		TILES_ASSERT(Ptr >= Base, "Vertex buffer pointer underflow");

		glm::mat4 translation = glm::translate(glm::mat4(1.0f), params.Position);
		glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), params.Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), params.Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), params.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 rotation = rotationZ * rotationY * rotationX;
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(params.Radius.x, params.Radius.y, 1.0f));
		glm::mat4 transform = translation * rotation * scale;

		float texIndex = state.Textures.ComputeTextureIndex(params.Texture);

		glm::vec2 uvMin = { params.TexCoords.x, params.TexCoords.y };
		glm::vec2 uvMax = { params.TexCoords.z, params.TexCoords.w };

		glm::vec2 uvs[4] =
		{
			{ uvMin.x, uvMax.y },  // Top-left
			{ uvMax.x, uvMax.y },  // Top-right
			{ uvMax.x, uvMin.y },  // Bottom-right
			{ uvMin.x, uvMin.y }   // Bottom-left
		};

		for (size_t i = 0; i < 4; i++)
		{
			Ptr->WorldPosition = transform * state.QuadVertexPositions[i];
			Ptr->LocalPosition = state.CircleVertexPositions[i];
			Ptr->Color = params.Color;
			Ptr->TexCoord = uvs[i];
			Ptr->TexIndex = texIndex;
			Ptr->Thickness = params.Thickness;
			Ptr->Fade = params.Fade;
			Ptr++;
		}

		IndexCount += 6;
		state.Stats.CircleCount++;
	}

	void CircleBatch::Reset()
	{
		IndexCount = 0;
		Ptr = Base;
	}

	bool CircleBatch::Upload(RendererState& state)
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

	void CircleBatch::Flush(RendererState& state)
	{
		if (IndexCount > 0)
		{
			Shader->Bind();
			SetSharedUniforms(Shader, state);

			VAO->Bind();
			RenderCommands::DrawElementsWithCount(VAO, PrimitiveType::Triangles, IndexCount);
			VAO->Unbind();
			Shader->Unbind();
			state.Stats.DrawCalls++;
		}

		IndexCount = 0;
	}
}
