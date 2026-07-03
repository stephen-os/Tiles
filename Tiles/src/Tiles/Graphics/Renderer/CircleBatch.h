#pragma once

#include "../Renderer.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace Tiles
{
	struct CircleVertex;
	struct RendererState;

	// Batches smooth (SDF-style) circles into a single indexed draw call.
	class CircleBatch
	{
	public:
		void Init(const std::vector<uint32_t>& quadIndices);
		void Shutdown();

		void Append(RendererState& state, const CircleParams& params);
		void Reset();
		bool Upload(RendererState& state);
		void Flush(RendererState& state);

		std::shared_ptr<VertexArray> VAO;
		std::shared_ptr<VertexBuffer> VBO;
		std::shared_ptr<IndexBuffer> IBO;
		std::shared_ptr<ShaderProgram> Shader;

		uint32_t IndexCount = 0;
		CircleVertex* Base = nullptr;
		CircleVertex* Ptr = nullptr;
	};
}
