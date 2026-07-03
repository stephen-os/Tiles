#pragma once

#include "../Renderer.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace Tiles
{
	struct QuadVertex;
	struct RendererState;

	// Batches textured/tinted quads into a single indexed draw call.
	class QuadBatch
	{
	public:
		void Init(const std::vector<uint32_t>& quadIndices);
		void Shutdown();

		void Append(RendererState& state, const QuadParams& params);
		void Reset();
		bool Upload(RendererState& state);
		void Flush(RendererState& state);

		std::shared_ptr<VertexArray> VAO;
		std::shared_ptr<VertexBuffer> VBO;
		std::shared_ptr<IndexBuffer> IBO;
		std::shared_ptr<ShaderProgram> Shader;

		uint32_t IndexCount = 0;
		QuadVertex* Base = nullptr;
		QuadVertex* Ptr = nullptr;
	};
}
