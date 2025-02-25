#pragma once

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

namespace Tiles
{

	class Camera
	{
	public:
		Camera() = default;
		~Camera() = default;
		void Drag(const glm::vec2& delta);
		void Zoom(const float delta);

		// Getters
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::vec2& GetPosition() { return m_Position; }
		float GetZoom() const { return m_Zoom; }

		// Setters
		void SetPosition(const glm::vec2& position) { m_Position = position; UpdateViewMatrix(); }
	private:
		void UpdateViewMatrix();
	private:
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::vec2 m_Position = glm::vec2(0.0f);

		float m_Sensitivity = 0.001f;
		float m_Zoom = 1.0f;
		float m_ZoomSpeed = 0.1f;
	};

}