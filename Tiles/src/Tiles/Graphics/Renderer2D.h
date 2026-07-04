#pragma once

#include <memory>
#include <cstdint>

#include "VertexArray.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "RenderCommands.h"
#include "RenderTarget.h"

#include <glm/glm.hpp>
#include <string>

namespace Tiles
{
	// Parameters for one quad draw. Set only what you need; the rest fall back to
	// sensible defaults (unit size, no rotation, full white tint, untextured).
	struct QuadParams
	{
		glm::vec3 Position{ 0.0f };
		glm::vec2 Size{ 1.0f };
		glm::vec3 Rotation{ 0.0f };
		glm::vec4 Tint{ 1.0f };
		std::shared_ptr<Texture> Texture = nullptr;
		glm::vec4 TexCoords{ 0.0f, 0.0f, 1.0f, 1.0f };
	};

	// Parameters for one line draw. Lines batch by thickness: drawing lines of
	// differing thickness flushes the batch between them (one GL width per batch).
	struct LineParams
	{
		glm::vec3 Start{ 0.0f };
		glm::vec3 End{ 0.0f };
		glm::vec4 Color{ 1.0f };
		float Thickness = 1.0f;
	};

	// Parameters for one circle draw. Radius is per-axis; Thickness and Fade
	// control the SDF ring (Thickness 1, Fade 0 renders a filled disc).
	struct CircleParams
	{
		glm::vec3 Position{ 0.0f };
		glm::vec2 Radius{ 1.0f };
		glm::vec3 Rotation{ 0.0f };
		glm::vec4 Color{ 1.0f };
		std::shared_ptr<Texture> Texture = nullptr;
		glm::vec4 TexCoords{ 0.0f, 0.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.0f;
	};

	// Configuration for one infinite-grid draw. The view-projection is taken from
	// the current frame (BeginFrame), so it is not part of the params.
	struct GridParams
	{
		float CellSize = 1.0f;          // minor cell size, world units
		float MajorEvery = 10.0f;       // a major line every N minor cells
		glm::vec2 Offset{ 0.0f };       // world-space shift of the grid origin
		glm::vec4 LineColor{ 0.30f, 0.30f, 0.33f, 1.0f };
		glm::vec4 MajorLineColor{ 0.50f, 0.50f, 0.55f, 1.0f };
		glm::vec4 BackgroundColor{ 0.13f, 0.13f, 0.15f, 1.0f };
	};

	// Immediate-mode 2D batch renderer. Primitives are drawn with DrawQuad/
	// DrawCircle/DrawLine(params) between BeginFrame() and EndFrame(); geometry accumulates into
	// per-primitive vertex buffers and is flushed in as few draw calls as
	// possible. All state is global (single static instance).
	class Renderer2D
	{
	public:

		// Initializes the renderer. Must be called before any other functions.
		static void Init();

		// Shuts down the renderer.
		static void Shutdown();

		// Begins a frame: sets the view-projection, binds and clears the current
		// render target, and starts a fresh batch.
		static void BeginFrame(const glm::mat4& viewProjection);
		
		// Ends the frame, flushing any pending geometry and unbinding 
		// the target.
		static void EndFrame();
		
		// Starts a new batch. Accumulates geometry until 
		// EndBatch() is called.
		static void StartBatch();
		
		// Uploads accumulated vertex data to the GPU and issues the 
		// draw calls.
		static void EndBatch();
		
		// Uploads accumulated vertex data to the GPU and issues the 
		// draw calls.
		static void Flush();

		// Sets the pixel resolution of the render target.
		static void SetResolution(uint32_t width, uint32_t height);

		// Sets the render mode.
		static void SetRenderMode(PolygonMode mode);

		// Draws one quad.
		static void DrawQuad(const QuadParams& params);

		// Draws one circle.
		static void DrawCircle(const CircleParams& params);
		
		// Draws one line.
		static void DrawLine(const LineParams& params);

		// Draws a fullscreen infinite grid behind everything (no depth write),
		// using the current frame's view-projection. Call after BeginFrame.
		static void DrawGrid(const GridParams& params);

		// Sets the current render target; a null target resets to the default.
		static void SetRenderTarget(std::shared_ptr<RenderTarget> target);

		// Returns the current render target.
		static std::shared_ptr<RenderTarget> GetCurrentRenderTarget();

		// Renderer2D statistics
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t CircleCount = 0;
			uint32_t LineCount = 0;
			uint32_t TexturesUsed = 0;
			uint32_t ShadersUsed = 0;
			uint32_t DataSize = 0;

			// Returns the total number of vertices used.
			uint32_t GetTotalVertexCount() const { return QuadCount * 4 + CircleCount * 4 + LineCount * 2; }
			
			// Returns the total number of indices used.
			uint32_t GetTotalIndexCount() const { return QuadCount * 6 + CircleCount * 6; }
		};

		// Returns the current statistics.
		static Statistics GetStats();

		// Resets the statistics.
		static void ResetStats();

	private:

		// Computes the index of the texture in the texture slot manager.
		static float ComputeTextureIndex(const std::shared_ptr<Texture>& texture);
	};
}
