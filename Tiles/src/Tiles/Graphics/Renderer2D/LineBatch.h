#pragma once

#include "../Renderer2D.h"

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

		void Append(RendererState& state);
		void Reset();
		bool Upload(RendererState& state);
		void Flush(RendererState& state);

		std::shared_ptr<VertexArray> VAO;
		std::shared_ptr<VertexBuffer> VBO;
		std::shared_ptr<ShaderProgram> Shader;

		uint32_t VertexCount = 0;
		LineVertex* Base = nullptr;
		LineVertex* Ptr = nullptr;

		// Width is the GL line width applied to the current batch; Thickness is the
		// value staged by SetLineThickness for the next line(s).
		float Width = 3.0f;

		glm::vec3 Start = { 0.0f, 0.0f, 0.0f };
		glm::vec3 End = { 1.0f, 1.0f, 0.0f };
		float Thickness = 2.0f;
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};
}
