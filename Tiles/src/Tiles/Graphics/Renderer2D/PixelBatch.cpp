#include "RendererState.h"

#include "Utils/FileReader.h"

namespace Tiles
{
	void PixelBatch::Init()
	{
		VAO = VertexArray::Create();
		VBO = VertexBuffer::Create(MaxPixels * sizeof(PixelVertex));
		VBO->SetLayout({
			{ BufferDataType::Float3, "a_Position" },
			{ BufferDataType::Float4, "a_Color" }
			});
		VAO->SetVertexBuffer(VBO);
		Base = new PixelVertex[MaxPixels];

		{
			std::string vertexSource = ReadFile("res/shaders/Pixel.vert");
			std::string fragmentSource = ReadFile("res/shaders/Pixel.frag");
			Shader = ShaderProgram::Create(vertexSource, fragmentSource);
		}
	}

	void PixelBatch::Shutdown()
	{
		VAO.reset();
		VBO.reset();
		Shader.reset();

		delete[] Base;
		Base = nullptr;
		Ptr = nullptr;
	}

	void PixelBatch::Append(RendererState& state)
	{
		Ptr->Position = Position;
		Ptr->Color = Color;
		Ptr++;
		VertexCount++;

		state.Stats.PixelCount++;
	}

	void PixelBatch::Reset()
	{
		VertexCount = 0;
		Ptr = Base;
	}

	bool PixelBatch::Upload(RendererState& state)
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

	void PixelBatch::Flush(RendererState& state)
	{
		if (VertexCount > 0)
		{
			Shader->Bind();
			Shader->SetUniformMat4("u_ViewProjection", state.ViewProjectionMatrix);
			Shader->SetUniformInt("u_EnableLighting", (int)state.UseLighting);
			Shader->SetUniformVec3("u_AmbientColor", state.AmbientColor);
			Shader->SetUniformFloat("u_AmbientIntensity", state.AmbientIntensity);
			Shader->SetUniformInt("u_PointLightCount", (int)state.PointLights.Count);

			VAO->Bind();
			RenderCommands::SetPointSize(Size);
			RenderCommands::DrawPoints(VAO, VertexCount);
			RenderCommands::SetPointSize(1.0f);
			VAO->Unbind();
			Shader->Unbind();
			state.Stats.DrawCalls++;
		}

		VertexCount = 0;
	}
}
