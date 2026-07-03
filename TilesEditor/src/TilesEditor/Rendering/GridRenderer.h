#pragma once

#include "Graphics/VertexArray.h"
#include "Graphics/ShaderProgram.h"

#include <glm/glm.hpp>
#include <memory>

namespace Tiles::Editor
{
	/// Configuration for one infinite-grid draw.
	struct GridParams
	{
		glm::mat4 ViewProjection{ 1.0f };
		float CellSize = 1.0f;         // minor cell size, world units
		float MajorEvery = 10.0f;      // a major line every N minor cells
		glm::vec4 LineColor{ 0.30f, 0.30f, 0.33f, 1.0f };
		glm::vec4 MajorLineColor{ 0.50f, 0.50f, 0.55f, 1.0f };
		glm::vec4 BackgroundColor{ 0.13f, 0.13f, 0.15f, 1.0f };
	};

	/// Renders a fullscreen procedural infinite grid from engine primitives. Each
	/// fragment reconstructs its world position from the inverse view-projection,
	/// so the grid is a function of world space: infinite and resolution
	/// independent (fwidth-based anti-aliasing).
	class GridRenderer
	{
	public:
		GridRenderer();   // creates GL resources; a GL context must be current

		/// Draws the grid across the viewport without writing depth, so anything
		/// drawn afterward sits on top. Call between Renderer2D::BeginFrame/EndFrame.
		void Draw(const GridParams& params);

	private:
		std::shared_ptr<Tiles::VertexArray> m_VAO;
		std::shared_ptr<Tiles::VertexBuffer> m_VBO;
		std::shared_ptr<Tiles::ShaderProgram> m_Shader;
	};
}
