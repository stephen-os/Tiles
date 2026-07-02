#pragma once

#include "../Renderer2D.h"

#include <glm/glm.hpp>
#include <memory>

namespace Tiles
{
	struct PixelVertex;
	struct RendererState;

	// Batches single points rendered with a shared GL point size. A change in
	// point size forces a whole-renderer flush (orchestrated by the facade).
	class PixelBatch
	{
	public:
		void Init();
		void Shutdown();

		void Append(RendererState& state);
		void Reset();
		bool Upload(RendererState& state);
		void Flush(RendererState& state);

		std::shared_ptr<VertexArray> VAO;
		std::shared_ptr<VertexBuffer> VBO;
		std::shared_ptr<ShaderProgram> Shader;

		uint32_t VertexCount = 0;
		PixelVertex* Base = nullptr;
		PixelVertex* Ptr = nullptr;

		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Size = 1.0f;
	};
}
