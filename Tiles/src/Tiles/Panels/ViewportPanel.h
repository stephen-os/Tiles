#pragma once

#include "Lumina/Renderer/Renderer.h"
#include "Lumina/Renderer/Buffer.h"
#include "Lumina/Renderer/VertexArray.h"
#include "Lumina/Renderer/ShaderProgram.h"

#include "Lumina/Renderer/OrthographicCamera.h"

#include "../Core/Base.h"

namespace Tiles
{
	class ViewportPanel
	{
	public:
		ViewportPanel();
		void Render();
	private:
		Lumina::Renderer m_Renderer;
		Shared<Lumina::VertexArray> m_Grid;
		Shared<Lumina::ShaderProgram> m_GridShader;
		OrthographicCamera m_Camera; // Not in namespace for some reason
	};
}