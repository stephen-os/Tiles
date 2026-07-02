#pragma once

#include "../Renderer2D.h"

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Tiles
{
	struct TextVertex;
	struct RendererState;

	// Batches bitmap-font glyph quads into a single indexed draw call.
	class TextBatch
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
		TextVertex* Base = nullptr;
		TextVertex* Ptr = nullptr;

		std::shared_ptr<Texture> DefaultFont = nullptr;

		std::string Content = "";
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Size = 1.0f;
		std::shared_ptr<Texture> Font = nullptr;
		StringAlignment Alignment = StringAlignment::Left;
	};
}
