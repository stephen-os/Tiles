#include "Renderer2D.h"
#include "Core/Log.h"

#include "Renderer2D/RendererState.h"

#include <memory>
#include <vector>
#include <cstring>

namespace Tiles
{
	// Single owner of all renderer state. Constructed in Init() once a GL context
	// is current, destroyed at the end of Shutdown(). The class methods below are a
	// thin facade over this state and its per-primitive batchers.
	static std::unique_ptr<RendererState> s_State;

	void Renderer2D::Init()
	{
		TILES_ENGINE_INFO("Renderer2D: Initializing...");

		s_State = std::make_unique<RendererState>();

		s_State->DefaultRenderTarget = RenderTarget::Create(800, 600);
		s_State->CurrentRenderTarget = s_State->DefaultRenderTarget;

		// Quad index pattern reused by every indexed batcher (quad/circle/grid).
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

		s_State->Quad.Init(quadIndices);
		s_State->Circle.Init(quadIndices);
		s_State->Line.Init();
		s_State->Grid.Init(quadIndices);

		s_State->Textures.Init();
		// Slot overflow flushes the whole renderer so the new texture starts fresh.
		s_State->Textures.SetFlushCallback([]() { EndBatch(); StartBatch(); });

		s_State->QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_State->QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_State->QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_State->QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_State->CircleVertexPositions[0] = { -1.0f, -1.0f, 0.0f };
		s_State->CircleVertexPositions[1] = { 1.0f, -1.0f, 0.0f };
		s_State->CircleVertexPositions[2] = { 1.0f,  1.0f, 0.0f };
		s_State->CircleVertexPositions[3] = { -1.0f,  1.0f, 0.0f };

		s_State->TexCoords[0] = { 0.0f, 0.0f };
		s_State->TexCoords[1] = { 1.0f, 0.0f };
		s_State->TexCoords[2] = { 1.0f, 1.0f };
		s_State->TexCoords[3] = { 0.0f, 1.0f };

		TILES_ENGINE_INFO("Renderer2D: Initialization complete");
	}

	void Renderer2D::Shutdown()
	{
		TILES_ENGINE_INFO("Renderer2D: Shutting down...");

		s_State->DefaultRenderTarget.reset();
		s_State->CurrentRenderTarget.reset();

		s_State->Quad.Shutdown();
		s_State->Circle.Shutdown();
		s_State->Line.Shutdown();
		s_State->Grid.Shutdown();

		s_State->Textures.Shutdown();

		s_State.reset();

		TILES_ENGINE_INFO("Renderer2D: Shutdown complete");
	}

	void Renderer2D::Begin(std::shared_ptr<OrthographicCamera> camera)
	{
		s_State->ViewProjectionMatrix = camera->GetProjectionMatrix() * camera->GetViewMatrix();

		s_State->CurrentRenderTarget->Bind();
		s_State->CurrentRenderTarget->Resize(s_State->Width, s_State->Height);

		RenderCommands::Clear();
		RenderCommands::SetViewport(0, 0, s_State->Width, s_State->Height);
		RenderCommands::EnableDepthTest();
		RenderCommands::SetPolygonMode(s_State->PolygonMode);

		StartBatch();
	}

	void Renderer2D::Begin(glm::mat4& viewProjection)
	{
		s_State->ViewProjectionMatrix = viewProjection;
		StartBatch();
	}

	void Renderer2D::End()
	{
		EndBatch();
		s_State->CurrentRenderTarget->Unbind();
	}

	void Renderer2D::StartBatch()
	{
		s_State->Quad.Reset();
		s_State->Circle.Reset();
		s_State->Line.Reset();
		s_State->Grid.Reset();

		s_State->Textures.Reset();
	}

	// Uploads whatever geometry each primitive accumulated this batch, then
	// flushes once if any primitive produced vertices.
	void Renderer2D::EndBatch()
	{
		bool issueDraw = false;

		if (s_State->Quad.Upload(*s_State)) issueDraw = true;
		if (s_State->Circle.Upload(*s_State)) issueDraw = true;
		if (s_State->Line.Upload(*s_State)) issueDraw = true;
		if (s_State->Grid.Upload(*s_State)) issueDraw = true;

		if (issueDraw)
			Flush();
	}

	// Binds textures once, then draws each non-empty primitive batch with its
	// shader and shared uniforms, and resets the per-batch counters.
	void Renderer2D::Flush()
	{
		s_State->Textures.Bind();

		s_State->Quad.Flush(*s_State);
		s_State->Circle.Flush(*s_State);
		s_State->Line.Flush(*s_State);
		s_State->Grid.Flush(*s_State);

		s_State->Textures.Unbind();

		s_State->Stats.TexturesUsed = s_State->Textures.Index - 1;
	}

	void Renderer2D::SetResolution(uint32_t width, uint32_t height)
	{
		s_State->Width = width;
		s_State->Height = height;
	}

	void Renderer2D::SetResolution(float width, float height)
	{
		SetResolution(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	}

	void Renderer2D::SetResolution(const glm::vec2& resolution)
	{
		SetResolution(static_cast<uint32_t>(resolution.x), static_cast<uint32_t>(resolution.y));
	}

	glm::vec2 Renderer2D::GetResolution()
	{
		return { s_State->Width, s_State->Height };
	}

	void Renderer2D::SetRenderMode(PolygonMode mode)
	{
		s_State->PolygonMode = mode;
	}

	void* Renderer2D::GetImage()
	{
		return reinterpret_cast<void*>(static_cast<uintptr_t>(s_State->CurrentRenderTarget->GetTexture()));
	}

	float Renderer2D::ComputeTextureIndex(const std::shared_ptr<Texture>& texture)
	{
		return s_State->Textures.ComputeTextureIndex(texture);
	}

	void Renderer2D::SetGridPosition(const glm::vec3& position)
	{
		s_State->Grid.Position = position;
	}

	void Renderer2D::SetGridRotation(const glm::vec3& rotation)
	{
		s_State->Grid.Rotation = rotation;
	}

	void Renderer2D::SetGridSize(const glm::vec2& size)
	{
		s_State->Grid.Size = size;
	}

	void Renderer2D::SetGridCellSize(float gridSize)
	{
		s_State->Grid.CellSize = gridSize;
	}

	void Renderer2D::SetGridColor(const glm::vec4& color)
	{
		s_State->Grid.Color = color;
	}

	void Renderer2D::SetGridLineWidth(float lineWidth)
	{
		s_State->Grid.LineWidth = lineWidth;
	}

	void Renderer2D::SetGridShowCheckerboard(bool showCheckerboard)
	{
		s_State->Grid.ShowCheckerboard = showCheckerboard;
	}

	void Renderer2D::SetGridCheckerColor1(const glm::vec4& checkerColor1)
	{
		s_State->Grid.CheckerColor1 = checkerColor1;
	}

	void Renderer2D::SetGridCheckerColor2(const glm::vec4& checkerColor2)
	{
		s_State->Grid.CheckerColor2 = checkerColor2;
	}

	void Renderer2D::ResetGridState()
	{
		s_State->Grid.Position = { 0.0f, 0.0f, 0.0f };
		s_State->Grid.Rotation = { 0.0f, 0.0f, 0.0f };
		s_State->Grid.Size = { 1000.0f, 1000.0f };
		s_State->Grid.CellSize = 1.0f;
		s_State->Grid.Color = { 0.3f, 0.3f, 0.3f, 0.8f };
		s_State->Grid.LineWidth = 1.0f;
		s_State->Grid.ShowCheckerboard = true;
		s_State->Grid.CheckerColor1 = { 0.9f, 0.9f, 0.9f, 0.2f };
		s_State->Grid.CheckerColor2 = { 0.8f, 0.8f, 0.8f, 0.2f };
	}

	void Renderer2D::DrawQuad(const QuadParams& params)
	{
		if (s_State->Quad.IndexCount >= MaxIndices)
		{
			EndBatch();
			StartBatch();
		}

		s_State->Quad.Append(*s_State, params);
	}

	void Renderer2D::DrawCircle(const CircleParams& params)
	{
		if (s_State->Circle.IndexCount >= MaxIndices)
		{
			EndBatch();
			StartBatch();
		}

		s_State->Circle.Append(*s_State, params);
	}

	void Renderer2D::DrawLine(const LineParams& params)
	{
		if (s_State->Line.VertexCount >= MaxVertices)
		{
			EndBatch();
			StartBatch();
		}

		if (s_State->Line.Width != params.Thickness)
		{
			EndBatch();
			StartBatch();
			s_State->Line.Width = params.Thickness;
		}

		s_State->Line.Append(*s_State, params);
	}

	void Renderer2D::DrawGrid()
	{
		if (s_State->Grid.IndexCount >= MaxIndices)
		{
			EndBatch();
			StartBatch();
		}

		s_State->Grid.Append(*s_State);
	}

	void Renderer2D::SetRenderTarget(std::shared_ptr<RenderTarget> target)
	{
		if (target)
		{
			s_State->CurrentRenderTarget = target;
		}
		else
		{
			s_State->CurrentRenderTarget = s_State->DefaultRenderTarget;
		}
	}

	void Renderer2D::SetRenderTarget(std::nullptr_t)
	{
		s_State->CurrentRenderTarget = s_State->DefaultRenderTarget;
	}

	std::shared_ptr<RenderTarget> Renderer2D::GetCurrentRenderTarget()
	{
		return s_State->CurrentRenderTarget;
	}

	std::shared_ptr<RenderTarget> Renderer2D::CreateRenderTarget(uint32_t width, uint32_t height)
	{
		return RenderTarget::Create(width, height);
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_State->Stats;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_State->Stats, 0, sizeof(Statistics));
	}
}
