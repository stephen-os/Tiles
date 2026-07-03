#pragma once

#include "../Renderer2D.h"

#include "Core/Assert.h"

#include <glm/glm.hpp>
#include <array>
#include <cstddef>
#include <cstdint>

namespace Tiles
{
	constexpr uint32_t MaxQuads = 10000;
	constexpr uint32_t MaxCircles = 10000;
	constexpr uint32_t MaxLines = 10000;
	constexpr uint32_t MaxGrids = 1000;

	constexpr uint32_t MaxVertices = MaxQuads * 4;
	constexpr uint32_t MaxIndices = MaxQuads * 6;
	constexpr uint32_t MaxTextureSlots = 32;

	struct QuadVertex
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

	struct GridVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		glm::vec3 GridPosition;
		glm::vec2 GridSize;
		float CellSize;
		glm::vec4 GridColor;
		float LineWidth;
		float ShowCheckerboard;
		glm::vec4 CheckerColor1;
		glm::vec4 CheckerColor2;
	};

	// Byte distance a write pointer advanced past its base; used to size buffer
	// uploads. Asserts the pointer is in range and the size fits a uint32_t.
	inline uint32_t CalculateBufferSize(const void* ptr, const void* base)
	{
		ptrdiff_t diff = static_cast<const uint8_t*>(ptr) - static_cast<const uint8_t*>(base);
		TILES_ASSERT(diff >= 0, "Renderer2D: Buffer pointer is before base pointer!");
		TILES_ASSERT(diff <= UINT32_MAX, "Renderer2D: Buffer size exceeds uint32_t limit!");
		return static_cast<uint32_t>(diff);
	}
}

#include "TextureSlotManager.h"
#include "SharedRenderUniforms.h"
#include "QuadBatch.h"
#include "CircleBatch.h"
#include "LineBatch.h"
#include "GridBatch.h"

namespace Tiles
{
	// Aggregates all renderer state behind the Renderer2D facade: shared frame
	// state, the shared quad geometry, statistics, the texture slot manager, and
	// one batcher per primitive. A single instance lives as a file-static in
	// Renderer2D.cpp, constructed in Init() once a GL context is current.
	struct RendererState
	{
		std::shared_ptr<RenderTarget> DefaultRenderTarget;
		std::shared_ptr<RenderTarget> CurrentRenderTarget;

		glm::mat4 ViewProjectionMatrix = glm::mat4(1.0f);

		uint32_t Width = 800;
		uint32_t Height = 600;

		PolygonMode PolygonMode = PolygonMode::Fill;
		glm::vec3 WireFrameColor = { 0.0f, 1.0f, 0.0f };

		// Unit-quad corners shared by the quad, circle, and grid batchers.
		glm::vec4 QuadVertexPositions[4];
		glm::vec3 CircleVertexPositions[4];
		glm::vec2 TexCoords[4];

		Renderer2D::Statistics Stats;

		TextureSlotManager Textures;

		QuadBatch Quad;
		CircleBatch Circle;
		LineBatch Line;
		GridBatch Grid;
	};
}
