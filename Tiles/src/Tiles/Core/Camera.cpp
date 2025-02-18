#include "Camera.h"

#include <algorithm>

namespace Tiles
{
	void Camera::Drag(const glm::vec2 delta)
	{
		m_Position -= glm::vec2(-delta.x * m_Sensitivity, -delta.y * m_Sensitivity);
		UpdateViewMatrix();
	}

	void Camera::Zoom(float delta)
	{
		// Maybe dont hard code the max and min values. 
		m_Zoom = std::min(2.0f, std::max(0.5f, m_Zoom + delta * m_ZoomSpeed));
		UpdateViewMatrix();
	}

	void Camera::UpdateViewMatrix()
	{
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(m_Position.x, m_Position.y, 0.0f));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(m_Zoom, m_Zoom, 1.0f));

		m_ViewMatrix = translation * scale;
	}
}