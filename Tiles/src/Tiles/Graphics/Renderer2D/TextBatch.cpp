#include "RendererState.h"

#include "Utils/FileReader.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Tiles
{
	void TextBatch::Init(const std::vector<uint32_t>& quadIndices)
	{
		VAO = VertexArray::Create();
		VBO = VertexBuffer::Create(MaxVertices * sizeof(TextVertex));
		VBO->SetLayout({
			{ BufferDataType::Float3, "a_Position" },
			{ BufferDataType::Float4, "a_Color" },
			{ BufferDataType::Float2, "a_TexCoord" },
			{ BufferDataType::Float,  "a_TexIndex" }
			});
		VAO->SetVertexBuffer(VBO);

		IBO = IndexBuffer::Create(const_cast<uint32_t*>(quadIndices.data()), MaxIndices);
		VAO->SetIndexBuffer(IBO);
		Base = new TextVertex[MaxVertices];

		{
			std::string vertexSource = ReadFile("res/shaders/Text.vert");
			std::string fragmentSource = ReadFile("res/shaders/Text.frag");
			Shader = ShaderProgram::Create(vertexSource, fragmentSource);
		}
	}

	void TextBatch::Shutdown()
	{
		VAO.reset();
		VBO.reset();
		IBO.reset();
		Shader.reset();

		delete[] Base;
		Base = nullptr;
		Ptr = nullptr;
	}

	void TextBatch::Append(RendererState& state)
	{
		std::shared_ptr<Texture> fontToUse = Font ? Font : DefaultFont;
		float texIndex = state.Textures.ComputeTextureIndex(fontToUse);

		float charWidth = Size;
		float charHeight = Size;

		// Calculate total text width for alignment
		float totalWidth = Content.length() * charWidth;

		// Calculate starting X offset based on alignment
		float startXOffset = 0.0f;
		switch (Alignment)
		{
		case StringAlignment::Left:
			startXOffset = 0.0f;
			break;
		case StringAlignment::Right:
			startXOffset = -totalWidth;
			break;
		case StringAlignment::Center:
			startXOffset = -totalWidth * 0.5f;
			break;
		}

		float xOffset = startXOffset;

		for (char c : Content)
		{
			glm::vec3 charPos = Position + glm::vec3(xOffset, 0.0f, 0.0f);

			glm::mat4 translation = glm::translate(glm::mat4(1.0f), charPos);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(charWidth, charHeight, 1.0f));
			glm::mat4 transform = translation * scale;

			float uvX = (c % 16) / 16.0f;
			float uvY = (c / 16) / 16.0f;
			float uvWidth = 1.0f / 16.0f;
			float uvHeight = 1.0f / 16.0f;

			glm::vec2 uvs[4] =
			{
				{ uvX, uvY + uvHeight },           // Top-left
				{ uvX + uvWidth, uvY + uvHeight }, // Top-right
				{ uvX + uvWidth, uvY },            // Bottom-right
				{ uvX, uvY }                       // Bottom-left
			};

			for (size_t i = 0; i < 4; i++)
			{
				Ptr->Position = transform * state.QuadVertexPositions[i];
				Ptr->Color = Color;
				Ptr->TexCoord = uvs[i];
				Ptr->TexIndex = texIndex;
				Ptr++;
			}

			IndexCount += 6;
			xOffset += charWidth;
		}

		state.Stats.TextCount++;
	}

	void TextBatch::Reset()
	{
		IndexCount = 0;
		Ptr = Base;
	}

	bool TextBatch::Upload(RendererState& state)
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

	void TextBatch::Flush(RendererState& state)
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
