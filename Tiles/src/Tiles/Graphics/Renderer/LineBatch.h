#pragma once

#include "../Renderer.h"

#include <glm/glm.hpp>
#include <memory>

namespace Tiles
{
	struct LineVertex;
	struct RendererState;

	// Batches line segments drawn with a shared GL line width. A change in line
	// width forces a whole-renderer flush (orchestrated by the facade) so each
	// batch renders with a single width.
	class LineBatch
	{
	public:
		void Init();
		void Shutdown();

		void Append(RendererState& state, const LineParams& params);
		void Reset();
		bool Upload(RendererState& state);
		void Flush(RendererState& state);

		std::shared_ptr<VertexArray> VAO;
		std::shared_ptr<VertexBuffer> VBO;
		std::shared_ptr<ShaderProgram> Shader;

		uint32_t VertexCount = 0;
		LineVertex* Base = nullptr;
		LineVertex* Ptr = nullptr;

		// GL line width applied to the current batch. DrawLine flushes when a new
		// line's thickness differs, so every batch renders at one width.
		float Width = 3.0f;
	};
}
