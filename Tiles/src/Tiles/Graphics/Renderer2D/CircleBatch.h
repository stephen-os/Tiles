#pragma once

#include "../Renderer2D.h"

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

		void Append(RendererState& state);
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

		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec2 Radius = { 1.0f, 1.0f };
		std::shared_ptr<Texture> Texture = nullptr;
		glm::vec4 TextureCoords = { 0.0f, 0.0f, 1.0f, 1.0f };
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.0f;
	};
}
