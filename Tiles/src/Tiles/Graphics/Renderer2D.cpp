#include "Renderer2D.h"

#include "Core/Assert.h"
#include "Core/Logger.h"

#include "Utils/FileReader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

namespace Tiles
{
	namespace
	{
		constexpr uint32_t MaxSquares = 10000;
		constexpr uint32_t MaxVertices = MaxSquares * 4;
		constexpr uint32_t MaxIndices = MaxSquares * 6;
		constexpr uint32_t MaxTextureSlots = 32;

		// Per-vertex layouts of the three batched primitives.
		struct SquareVertex
		{
			glm::vec3 Position;
			glm::vec4 Color;
			glm::vec2 TexCoord;
			float TexIndex;
		};

		struct CircleVertex
		{
			glm::vec3 WorldPosition;
			glm::vec3 LocalPosition;
			glm::vec4 Color;
			glm::vec2 TexCoord;
			float TexIndex;
			float Thickness;
			float Fade;
		};

		struct LineVertex
		{
			glm::vec3 Position;
			glm::vec4 Color;
		};

		// One accumulating batch: a vertex array + its buffers, a shader, and a
		// CPU staging buffer the Draw* calls write into through Ptr. Squares and
		// circles are indexed (two triangles per quad); lines are drawn directly.
		struct SquareData
		{
			std::shared_ptr<VertexArray> VAO;
			std::shared_ptr<VertexBuffer> VBO;
			std::shared_ptr<IndexBuffer> IBO;
			std::shared_ptr<ShaderProgram> Shader;

			std::vector<SquareVertex> Vertices;
			SquareVertex* Ptr = nullptr;
			uint32_t IndexCount = 0;
		};

		struct CircleData
		{
			std::shared_ptr<VertexArray> VAO;
			std::shared_ptr<VertexBuffer> VBO;
			std::shared_ptr<IndexBuffer> IBO;
			std::shared_ptr<ShaderProgram> Shader;

			std::vector<CircleVertex> Vertices;
			CircleVertex* Ptr = nullptr;
			uint32_t IndexCount = 0;
		};

		struct LineData
		{
			std::shared_ptr<VertexArray> VAO;
			std::shared_ptr<VertexBuffer> VBO;
			std::shared_ptr<ShaderProgram> Shader;

			std::vector<LineVertex> Vertices;
			LineVertex* Ptr = nullptr;
			uint32_t VertexCount = 0;

			// GL line width for the current batch. A new thickness forces a flush
			// so every batch renders at a single width.
			float Width = 3.0f;
		};

		struct GridData
		{
			std::shared_ptr<VertexArray> VAO;
			std::shared_ptr<VertexBuffer> VBO;
			std::shared_ptr<ShaderProgram> Shader;
		};

		// All renderer state. Held behind a unique_ptr and constructed in Init()
		// once a GL context is current, so no GL objects are created at static-init.
		struct RendererData
		{
			std::shared_ptr<RenderTarget> DefaultRenderTarget;
			std::shared_ptr<RenderTarget> CurrentRenderTarget;

			glm::mat4 ViewProjectionMatrix = glm::mat4(1.0f);

			uint32_t Width = 800;
			uint32_t Height = 600;

			PolygonMode PolygonMode = PolygonMode::Fill;
			glm::vec3 WireframeColor = { 0.0f, 1.0f, 0.0f };

			// Unit-quad corners shared by the square and circle batchers: the
			// world-space square corners and the circle's local (SDF) corners.
			glm::vec4 QuadPositions[4];
			glm::vec3 CircleLocalPositions[4];

			SquareData Square;
			CircleData Circle;
			LineData Line;
			GridData Grid;

			// Texture sampler slots. Slot 0 is a 1x1 white texture and the result
			// for a null texture; the rest are assigned on first use.
			std::array<std::shared_ptr<Texture>, MaxTextureSlots> TextureSlots;
			uint32_t TextureSlotIndex = 1;

			Renderer2D::Statistics Stats;
		};

		std::unique_ptr<RendererData> s_Data;

		// Byte distance a write pointer advanced past its base; sizes an upload.
		uint32_t UsedBytes(const void* ptr, const void* base)
		{
			ptrdiff_t diff = static_cast<const uint8_t*>(ptr) - static_cast<const uint8_t*>(base);
			TILES_ASSERT(diff >= 0, "Renderer2D: staging pointer is before its base!");
			TILES_ASSERT(diff <= UINT32_MAX, "Renderer2D: staging size exceeds uint32_t!");
			return static_cast<uint32_t>(diff);
		}

		// Applies the view-projection and wireframe uniforms shared by the square,
		// circle, and line shaders.
		void ApplySharedUniforms(const std::shared_ptr<ShaderProgram>& shader)
		{
			shader->SetUniformMat4("u_ViewProjection", s_Data->ViewProjectionMatrix);
			shader->SetUniformInt("u_WireframeMode", static_cast<int>(s_Data->PolygonMode));
			shader->SetUniformVec3("u_WireframeColor", s_Data->WireframeColor);
		}

		// Returns the sampler slot for a texture, assigning the next free slot on
		// first use. A null texture resolves to slot 0 (white). When the slot table
		// is full the batch is flushed so the next texture starts a fresh set.
		float ResolveTextureSlot(const std::shared_ptr<Texture>& texture)
		{
			if (texture == nullptr)
				return 0.0f;

			for (uint32_t i = 1; i < s_Data->TextureSlotIndex; i++)
			{
				if (s_Data->TextureSlots[i] == texture)
					return static_cast<float>(i);
			}

			if (s_Data->TextureSlotIndex >= MaxTextureSlots)
			{
				Renderer2D::EndBatch();
				Renderer2D::StartBatch();
			}

			TILES_ASSERT(s_Data->TextureSlotIndex < MaxTextureSlots, "Texture slot index overflow!");

			float slot = static_cast<float>(s_Data->TextureSlotIndex);
			s_Data->TextureSlots[s_Data->TextureSlotIndex] = texture;
			s_Data->TextureSlotIndex++;
			return slot;
		}

		// Binds every live texture slot to its sampler unit.
		void BindTextureSlots()
		{
			for (uint32_t i = 0; i < s_Data->TextureSlotIndex; i++)
				s_Data->TextureSlots[i]->Bind(i);
		}

		// Unbinds every live texture slot.
		void UnbindTextureSlots()
		{
			for (uint32_t i = 0; i < s_Data->TextureSlotIndex; i++)
				s_Data->TextureSlots[i]->Unbind();
		}
	}

	// Creates every batch's GPU objects and staging buffer and the white texture.
	std::expected<void, Error> Renderer2D::Init()
	{
		TILES_ENGINE_INFO("Renderer2D: Initializing...");

		s_Data = std::make_unique<RendererData>();

		s_Data->DefaultRenderTarget = RenderTarget::Create(800, 600);
		s_Data->CurrentRenderTarget = s_Data->DefaultRenderTarget;

		// Quad index pattern (two triangles per quad) shared by the square and
		// circle batchers.
		std::vector<uint32_t> quadIndices(MaxIndices);
		uint32_t offset = 0;
		for (uint32_t i = 0; i < MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;
			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;
			offset += 4;
		}

		// Square batch.
		{
			SquareData& square = s_Data->Square;
			square.VAO = VertexArray::Create();
			square.VBO = VertexBuffer::Create(MaxVertices * sizeof(SquareVertex));
			square.VBO->SetLayout({
				{ BufferDataType::Float3, "a_Position" },
				{ BufferDataType::Float4, "a_Color" },
				{ BufferDataType::Float2, "a_TexCoord" },
				{ BufferDataType::Float,  "a_TexIndex" }
				});
			square.VAO->SetVertexBuffer(square.VBO);
			square.IBO = IndexBuffer::Create(quadIndices.data(), MaxIndices);
			square.VAO->SetIndexBuffer(square.IBO);
			square.Vertices.resize(MaxVertices);
			square.Shader = ShaderProgram::Create(ReadFile("res/shaders/Quad.vert"), ReadFile("res/shaders/Quad.frag"));
		}

		// Circle batch.
		{
			CircleData& circle = s_Data->Circle;
			circle.VAO = VertexArray::Create();
			circle.VBO = VertexBuffer::Create(MaxVertices * sizeof(CircleVertex));
			circle.VBO->SetLayout({
				{ BufferDataType::Float3, "a_WorldPosition" },
				{ BufferDataType::Float3, "a_LocalPosition" },
				{ BufferDataType::Float4, "a_Color" },
				{ BufferDataType::Float2, "a_TexCoord" },
				{ BufferDataType::Float,  "a_TexIndex" },
				{ BufferDataType::Float,  "a_Thickness" },
				{ BufferDataType::Float,  "a_Fade" }
				});
			circle.VAO->SetVertexBuffer(circle.VBO);
			circle.IBO = IndexBuffer::Create(quadIndices.data(), MaxIndices);
			circle.VAO->SetIndexBuffer(circle.IBO);
			circle.Vertices.resize(MaxVertices);
			circle.Shader = ShaderProgram::Create(ReadFile("res/shaders/Circle.vert"), ReadFile("res/shaders/Circle.frag"));
		}

		// Line batch.
		{
			LineData& line = s_Data->Line;
			line.VAO = VertexArray::Create();
			line.VBO = VertexBuffer::Create(MaxVertices * sizeof(LineVertex));
			line.VBO->SetLayout({
				{ BufferDataType::Float3, "a_Position" },
				{ BufferDataType::Float4, "a_Color" }
				});
			line.VAO->SetVertexBuffer(line.VBO);
			line.Vertices.resize(MaxVertices);
			line.Shader = ShaderProgram::Create(ReadFile("res/shaders/Line.vert"), ReadFile("res/shaders/Line.frag"));
		}

		// Grid pass: a fullscreen clip-space quad drawn as a triangle strip.
		{
			GridData& grid = s_Data->Grid;
			float vertices[] =
			{
				-1.0f, -1.0f,
				 1.0f, -1.0f,
				-1.0f,  1.0f,
				 1.0f,  1.0f,
			};
			grid.VAO = VertexArray::Create();
			grid.VBO = VertexBuffer::Create(sizeof(vertices));
			grid.VBO->SetLayout({ { BufferDataType::Float2, "a_NDC" } });
			grid.VAO->SetVertexBuffer(grid.VBO);
			grid.VBO->SetData(vertices, sizeof(vertices));
			grid.Shader = ShaderProgram::Create(ReadFile("res/shaders/Grid.vert"), ReadFile("res/shaders/Grid.frag"));
		}

		// Slot 0 is a 1x1 white texture, used for untextured primitives.
		uint32_t whitePixel = 0xffffffff;
		s_Data->TextureSlots[0] = Texture::Create(1, 1);
		s_Data->TextureSlots[0]->SetData(&whitePixel, sizeof(uint32_t));

		// Every GL resource must have a non-zero handle; a zero means a buffer,
		// shader, texture, or target failed to create (details are already logged,
		// by the ctor and, in non-Dist builds, the GL debug callback).
		const bool created =
			s_Data->DefaultRenderTarget->GetTexture() != 0
			&& s_Data->Square.VAO->GetID() && s_Data->Square.VBO->GetID() && s_Data->Square.IBO->GetID() && s_Data->Square.Shader->GetID()
			&& s_Data->Circle.VAO->GetID() && s_Data->Circle.VBO->GetID() && s_Data->Circle.IBO->GetID() && s_Data->Circle.Shader->GetID()
			&& s_Data->Line.VAO->GetID() && s_Data->Line.VBO->GetID() && s_Data->Line.Shader->GetID()
			&& s_Data->Grid.VAO->GetID() && s_Data->Grid.VBO->GetID() && s_Data->Grid.Shader->GetID()
			&& s_Data->TextureSlots[0]->GetID() != 0;
		if (!created)
		{
			s_Data.reset();
			return std::unexpected(Error{ ErrorCode::ResourceCreationFailure, "Renderer2D: one or more GL resources failed to initialize (see log)." });
		}

		s_Data->QuadPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->QuadPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->QuadPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data->QuadPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Data->CircleLocalPositions[0] = { -1.0f, -1.0f, 0.0f };
		s_Data->CircleLocalPositions[1] = {  1.0f, -1.0f, 0.0f };
		s_Data->CircleLocalPositions[2] = {  1.0f,  1.0f, 0.0f };
		s_Data->CircleLocalPositions[3] = { -1.0f,  1.0f, 0.0f };

		TILES_ENGINE_INFO("Renderer2D: Initialization complete");
		return {};
	}

	// Destroys all renderer state (and with it every GL object, via RAII).
	void Renderer2D::Shutdown()
	{
		TILES_ENGINE_INFO("Renderer2D: Shutting down...");
		s_Data.reset();
		TILES_ENGINE_INFO("Renderer2D: Shutdown complete");
	}

	// Begins a frame: view-projection, target bind/clear, then a fresh batch.
	void Renderer2D::BeginFrame(const glm::mat4& viewProjection)
	{
		s_Data->ViewProjectionMatrix = viewProjection;

		s_Data->CurrentRenderTarget->Bind();
		s_Data->CurrentRenderTarget->Resize(s_Data->Width, s_Data->Height);

		RenderCommands::Clear();
		RenderCommands::SetViewport(0, 0, s_Data->Width, s_Data->Height);
		RenderCommands::EnableDepthTest();
		RenderCommands::SetPolygonMode(s_Data->PolygonMode);

		StartBatch();
	}

	// Ends the frame, flushing pending geometry and unbinding the target.
	void Renderer2D::EndFrame()
	{
		EndBatch();
		s_Data->CurrentRenderTarget->Unbind();
	}

	// Rewinds every batch's write cursor and the texture slots for a new batch.
	void Renderer2D::StartBatch()
	{
		s_Data->Square.IndexCount = 0;
		s_Data->Square.Ptr = s_Data->Square.Vertices.data();

		s_Data->Circle.IndexCount = 0;
		s_Data->Circle.Ptr = s_Data->Circle.Vertices.data();

		s_Data->Line.VertexCount = 0;
		s_Data->Line.Ptr = s_Data->Line.Vertices.data();

		s_Data->TextureSlotIndex = 1;
	}

	// Uploads whatever geometry each primitive accumulated, then flushes once if
	// any primitive produced vertices.
	void Renderer2D::EndBatch()
	{
		// Uploads a batch's staging buffer to its VBO; returns whether it drew.
		auto upload = [](auto& batch) -> bool
		{
			uint32_t size = UsedBytes(batch.Ptr, batch.Vertices.data());
			if (size == 0)
				return false;

			s_Data->Stats.DataSize += size;
			batch.VBO->SetData(batch.Vertices.data(), size);
			return true;
		};

		bool issueDraw = false;
		if (upload(s_Data->Square)) issueDraw = true;
		if (upload(s_Data->Circle)) issueDraw = true;
		if (upload(s_Data->Line))   issueDraw = true;

		if (issueDraw)
			Flush();
	}

	// Binds textures once, then draws each non-empty batch with its shader and
	// shared uniforms.
	void Renderer2D::Flush()
	{
		BindTextureSlots();

		if (s_Data->Square.IndexCount > 0)
		{
			SquareData& square = s_Data->Square;
			square.Shader->Bind();
			ApplySharedUniforms(square.Shader);
			square.VAO->Bind();
			RenderCommands::DrawElementsWithCount(square.VAO, PrimitiveType::Triangles, square.IndexCount);
			square.VAO->Unbind();
			square.Shader->Unbind();
			s_Data->Stats.DrawCalls++;
			square.IndexCount = 0;
		}

		if (s_Data->Circle.IndexCount > 0)
		{
			CircleData& circle = s_Data->Circle;
			circle.Shader->Bind();
			ApplySharedUniforms(circle.Shader);
			circle.VAO->Bind();
			RenderCommands::DrawElementsWithCount(circle.VAO, PrimitiveType::Triangles, circle.IndexCount);
			circle.VAO->Unbind();
			circle.Shader->Unbind();
			s_Data->Stats.DrawCalls++;
			circle.IndexCount = 0;
		}

		if (s_Data->Line.VertexCount > 0)
		{
			LineData& line = s_Data->Line;
			line.Shader->Bind();
			ApplySharedUniforms(line.Shader);
			line.VAO->Bind();
			RenderCommands::SetLineWidth(line.Width);
			RenderCommands::DrawLines(line.VAO, line.VertexCount);
			line.VAO->Unbind();
			line.Shader->Unbind();
			s_Data->Stats.DrawCalls++;
			line.VertexCount = 0;
		}

		UnbindTextureSlots();

		s_Data->Stats.TexturesUsed = s_Data->TextureSlotIndex - 1;
	}

	// Sets the pixel resolution applied to the render target next frame.
	void Renderer2D::SetResolution(uint32_t width, uint32_t height)
	{
		s_Data->Width = width;
		s_Data->Height = height;
	}

	// Sets the fill/wireframe polygon mode.
	void Renderer2D::SetRenderMode(PolygonMode mode)
	{
		s_Data->PolygonMode = mode;
	}

	// Appends one square (four vertices, six indices) to the batch.
	void Renderer2D::DrawSquare(const Square& square)
	{
		if (s_Data->Square.IndexCount >= MaxIndices)
		{
			EndBatch();
			StartBatch();
		}

		glm::mat4 translation = glm::translate(glm::mat4(1.0f), square.Position);
		glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(square.Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(square.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(square.Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 rotation = rotationZ * rotationY * rotationX;
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(square.Size, 1.0f));
		glm::mat4 transform = translation * rotation * scale;

		float texIndex = ResolveTextureSlot(square.Texture);

		glm::vec2 uvMin = { square.TexCoords.x, square.TexCoords.y };
		glm::vec2 uvMax = { square.TexCoords.z, square.TexCoords.w };
		glm::vec2 uvs[4] =
		{
			{ uvMin.x, uvMax.y },
			{ uvMax.x, uvMax.y },
			{ uvMax.x, uvMin.y },
			{ uvMin.x, uvMin.y }
		};

		SquareData& batch = s_Data->Square;
		for (size_t i = 0; i < 4; i++)
		{
			batch.Ptr->Position = transform * s_Data->QuadPositions[i];
			batch.Ptr->Color = square.Tint;
			batch.Ptr->TexCoord = uvs[i];
			batch.Ptr->TexIndex = texIndex;
			batch.Ptr++;
		}

		batch.IndexCount += 6;
		s_Data->Stats.SquareCount++;
	}

	// Appends one circle (a quad carrying SDF data) to the batch.
	void Renderer2D::DrawCircle(const Circle& circle)
	{
		if (s_Data->Circle.IndexCount >= MaxIndices)
		{
			EndBatch();
			StartBatch();
		}

		glm::mat4 translation = glm::translate(glm::mat4(1.0f), circle.Position);
		glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), circle.Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), circle.Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), circle.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 rotation = rotationZ * rotationY * rotationX;
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(circle.Radius.x, circle.Radius.y, 1.0f));
		glm::mat4 transform = translation * rotation * scale;

		float texIndex = ResolveTextureSlot(circle.Texture);

		glm::vec2 uvMin = { circle.TexCoords.x, circle.TexCoords.y };
		glm::vec2 uvMax = { circle.TexCoords.z, circle.TexCoords.w };
		glm::vec2 uvs[4] =
		{
			{ uvMin.x, uvMax.y },
			{ uvMax.x, uvMax.y },
			{ uvMax.x, uvMin.y },
			{ uvMin.x, uvMin.y }
		};

		CircleData& batch = s_Data->Circle;
		for (size_t i = 0; i < 4; i++)
		{
			batch.Ptr->WorldPosition = transform * s_Data->QuadPositions[i];
			batch.Ptr->LocalPosition = s_Data->CircleLocalPositions[i];
			batch.Ptr->Color = circle.Color;
			batch.Ptr->TexCoord = uvs[i];
			batch.Ptr->TexIndex = texIndex;
			batch.Ptr->Thickness = circle.Thickness;
			batch.Ptr->Fade = circle.Fade;
			batch.Ptr++;
		}

		batch.IndexCount += 6;
		s_Data->Stats.CircleCount++;
	}

	// Appends one line segment (two vertices). A thickness change flushes first so
	// each batch renders at a single GL line width.
	void Renderer2D::DrawLine(const Line& line)
	{
		if (s_Data->Line.VertexCount >= MaxVertices)
		{
			EndBatch();
			StartBatch();
		}

		if (s_Data->Line.Width != line.Thickness)
		{
			EndBatch();
			StartBatch();
			s_Data->Line.Width = line.Thickness;
		}

		LineData& batch = s_Data->Line;
		batch.Ptr->Position = line.Start;
		batch.Ptr->Color = line.Color;
		batch.Ptr++;
		batch.VertexCount++;

		batch.Ptr->Position = line.End;
		batch.Ptr->Color = line.Color;
		batch.Ptr++;
		batch.VertexCount++;

		s_Data->Stats.LineCount++;
	}

	// Draws the fullscreen infinite grid, leaving depth untouched so everything
	// drawn afterward sits on top.
	void Renderer2D::DrawGrid(const Grid& grid)
	{
		GridData& pass = s_Data->Grid;

		pass.Shader->Bind();
		pass.Shader->SetUniformMat4("u_InvViewProjection", glm::inverse(s_Data->ViewProjectionMatrix));
		pass.Shader->SetUniformFloat("u_CellSize", grid.CellSize);
		pass.Shader->SetUniformFloat("u_MajorEvery", grid.MajorEvery);
		pass.Shader->SetUniformVec2("u_GridOffset", grid.Offset);
		pass.Shader->SetUniformVec4("u_LineColor", grid.LineColor);
		pass.Shader->SetUniformVec4("u_MajorLineColor", grid.MajorLineColor);
		pass.Shader->SetUniformVec4("u_BackgroundColor", grid.BackgroundColor);

		RenderCommands::SetDepthMask(false);
		RenderCommands::DrawTriangleStrip(pass.VAO, 4);
		RenderCommands::SetDepthMask(true);

		pass.Shader->Unbind();

		s_Data->Stats.DrawCalls++;
	}

	// Sets the current render target; a null target resets to the default.
	void Renderer2D::SetRenderTarget(std::shared_ptr<RenderTarget> target)
	{
		s_Data->CurrentRenderTarget = target ? target : s_Data->DefaultRenderTarget;
	}

	// Returns the current render target.
	std::shared_ptr<RenderTarget> Renderer2D::GetCurrentRenderTarget()
	{
		return s_Data->CurrentRenderTarget;
	}

	// Returns a copy of the current frame statistics.
	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data->Stats;
	}

	// Zeroes the statistics.
	void Renderer2D::ResetStats()
	{
		s_Data->Stats = Statistics{};
	}
}
