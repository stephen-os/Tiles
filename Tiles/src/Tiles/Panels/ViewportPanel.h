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

		// Mouse interaction state
		bool m_IsMiddleMouseDown = false;
		glm::vec2 m_LastMousePos = { 0.0f, 0.0f };
		glm::vec2 m_CameraPosition = { 0.0f, 0.0f };

		float m_Sensitivity = 0.05f; // Adjust this value for the desired speed reduction
	};
}