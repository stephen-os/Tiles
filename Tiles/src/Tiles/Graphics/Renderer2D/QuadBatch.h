#pragma once

#include "../Renderer2D.h"

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

		void Append(RendererState& state);
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

		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec2 Size = { 1.0f, 1.0f };
		std::shared_ptr<Texture> Texture = nullptr;
		glm::vec4 TextureCoords = { 0.0f, 0.0f, 1.0f, 1.0f };
		glm::vec4 TintColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	};
}
