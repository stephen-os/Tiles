#pragma once

#include "../Renderer2D.h"

#include <memory>

namespace Tiles
{
	struct RendererState;

	// Renders a fullscreen procedural infinite grid. Each fragment reconstructs
	// its world position from the inverse of the frame's view-projection, so the
	// grid is a function of world space -- infinite and resolution independent
	// (fwidth-based anti-aliasing). Draws with depth writes off so it sits behind
	// everything drawn afterward.
	class GridPass
	{
	public:
		void Init();
		void Shutdown();
		void Draw(RendererState& state, const GridParams& params);

	private:
		std::shared_ptr<VertexArray> m_VAO;
		std::shared_ptr<VertexBuffer> m_VBO;
		std::shared_ptr<ShaderProgram> m_Shader;
	};
}
