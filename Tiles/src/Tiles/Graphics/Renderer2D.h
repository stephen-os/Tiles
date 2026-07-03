#pragma once
#include <memory>
#include <cstdint>

#include "VertexArray.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "RenderCommands.h"
#include "RenderTarget.h"

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

#include "Cameras/OrthographicCamera.h"

namespace Tiles
{
	enum class BlendMode : int
	{
		Additive = 0,        // Standard light blending (light adds to scene)
		Multiply = 1,        // Darkens areas (shadows/ambient occlusion)
		Screen = 2,          // Brightens without harsh highlights
		Overlay = 3,         // Combines multiply and screen
		SoftLight = 4,       // Subtle lighting effect
		LinearBurn = 5,      // Darker than multiply
		ColorDodge = 6,      // Creates bright highlights
		Subtract = 7,        // Subtracts light (negative lighting)
		Alpha = 8            // Standard alpha blending
	};

	enum class AttenuationModel : int
	{
		None = 0,            // No falloff (constant intensity)
		Linear = 1,          // Linear falloff
		Quadratic = 2,       // Quadratic falloff (physically accurate)
		InverseSquare = 3,   // 1/distance^2 (realistic)
		Exponential = 4,     // Exponential decay
		Smoothstep = 5,      // Smooth transition with ease-in/out
		Custom = 6,          // User-defined curve
		Realistic = 7        // Physically-based (constant + linear + quadratic)
	};

	enum class StringAlignment : int
	{
		Left = 0,
		Right = 1,
		Center = 2
	};

	/// Immediate-mode-style 2D batch renderer. Primitives are configured through
	/// Set*/Draw* calls between Begin() and End(); geometry accumulates into
	/// per-primitive vertex buffers and is flushed in as few draw calls as
	/// possible. All state is global (single static instance).
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		/// Begins a frame: sets the view-projection, binds the current render
		/// target, clears it, and starts a fresh batch.
		static void Begin(std::shared_ptr<OrthographicCamera> camera);
		/// Begins a frame with an explicit view-projection and no target setup.
		static void Begin(glm::mat4& viewProjection);
		/// Ends the frame, flushing any pending geometry and unbinding the target.
		static void End();

		static void StartBatch();
		/// Uploads accumulated vertex data to the GPU and issues the draw calls.
		static void EndBatch();
		static void Flush();

		static void SetResolution(uint32_t width, uint32_t height);
		static void SetResolution(float width, float height);
		static void SetResolution(const glm::vec2& resolution);
		static glm::vec2 GetResolution();

		static void SetRenderMode(PolygonMode mode);
		static void* GetImage();

		static void SetQuadPosition(const glm::vec3& position);
		static void SetQuadRotation(const glm::vec3& rotation);
		static void SetQuadSize(const glm::vec2& size);
		static void SetQuadTexture(const std::shared_ptr<Texture>& texture);
		static void SetQuadTextureCoords(const glm::vec4& textureCoords);
		static void SetQuadTintColor(const glm::vec4& tintColor);
		static void ResetQuadState();

		static void SetCirclePosition(const glm::vec3& position);
		static void SetCircleRotation(const glm::vec3& rotation);
		static void SetCircleRadius(const glm::vec2& radius);
		static void SetCircleTexture(const std::shared_ptr<Texture>& texture);
		static void SetCircleTextureCoords(const glm::vec4& textureCoords);
		static void SetCircleColor(const glm::vec4& color);
		static void SetCircleThickness(float thickness);
		static void SetCircleFade(float fade);
		static void ResetCircleState();

		static void SetLineStart(const glm::vec3& start);
		static void SetLineEnd(const glm::vec3& end);
		static void SetLineThickness(float thickness);
		static void SetLineColor(const glm::vec4& color);
		static void ResetLineState();

		static void SetGridPosition(const glm::vec3& position);
		static void SetGridRotation(const glm::vec3& rotation);
		static void SetGridSize(const glm::vec2& size);
		static void SetGridCellSize(float gridSize);
		static void SetGridColor(const glm::vec4& color);
		static void SetGridLineWidth(float lineWidth);
		static void SetGridShowCheckerboard(bool showCheckerboard);
		static void SetGridCheckerColor1(const glm::vec4& checkerColor1);
		static void SetGridCheckerColor2(const glm::vec4& checkerColor2);
		static void ResetGridState();

		static void DrawQuad();
		static void DrawCircle();
		static void DrawLine();
		static void DrawGrid();

		static void SetRenderTarget(std::shared_ptr<RenderTarget> target);
		static void SetRenderTarget(std::nullptr_t);
		static std::shared_ptr<RenderTarget> GetCurrentRenderTarget();
		static std::shared_ptr<RenderTarget> CreateRenderTarget(uint32_t width, uint32_t height);

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t CircleCount = 0;
			uint32_t LineCount = 0;
			uint32_t GridCount = 0;
			uint32_t TexturesUsed = 0;
			uint32_t ShadersUsed = 0;
			uint32_t DataSize = 0;

			uint32_t GetTotalVertexCount() const { return QuadCount * 4 + CircleCount * 4 + LineCount * 2 + GridCount * 4; }
			uint32_t GetTotalIndexCount() const { return QuadCount * 6 + CircleCount * 6 + GridCount * 6; }
		};

		static Statistics GetStats();
		static void ResetStats();

	private:
		static float ComputeTextureIndex(const std::shared_ptr<Texture>& texture);

	};
}
