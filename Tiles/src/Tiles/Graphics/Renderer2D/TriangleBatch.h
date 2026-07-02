#pragma once

#include "../Renderer2D.h"

#include <glm/glm.hpp>
#include <memory>

namespace Tiles
{
	struct TriangleVertex;
	struct RendererState;

	// Batches flat/textured triangles into a single non-indexed draw call.
	class TriangleBatch
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
		TriangleVertex* Base = nullptr;
		TriangleVertex* Ptr = nullptr;

		glm::vec3 Point1 = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Point2 = { 1.0f, 0.0f, 0.0f };
		glm::vec3 Point3 = { 0.5f, 1.0f, 0.0f };
		std::shared_ptr<Texture> Texture = nullptr;
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};
}
