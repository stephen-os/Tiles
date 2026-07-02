#pragma once

#include "../Renderer2D.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace Tiles
{
	struct GridVertex;
	struct RendererState;

	// Batches procedural grid/checkerboard quads into a single indexed draw call.
	// The grid shader receives only u_ViewProjection (no wireframe/lighting).
	class GridBatch
	{
	public:
		void Init(const std::vector<uint32_t>& quadIndices);
		void Shutdown();

		void Append(RendererState& state);
		void Reset();
		bool Upload(RendererState& state);
		void Flush(RendererState& state);

		std::shared_ptr<VertexArray> VAO;
		std::shared_ptr<VertexBuffer> VBO;
		std::shared_ptr<IndexBuffer> IBO;
		std::shared_ptr<ShaderProgram> Shader;

		uint32_t IndexCount = 0;
		GridVertex* Base = nullptr;
		GridVertex* Ptr = nullptr;

		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec2 Size = { 1000.0f, 1000.0f };
		float CellSize = 1.0f;
		glm::vec4 Color = { 0.3f, 0.3f, 0.3f, 0.8f };
		float LineWidth = 1.0f;
		bool ShowCheckerboard = true;
		glm::vec4 CheckerColor1 = { 0.9f, 0.9f, 0.9f, 0.2f };
		glm::vec4 CheckerColor2 = { 0.8f, 0.8f, 0.8f, 0.2f };
	};
}
