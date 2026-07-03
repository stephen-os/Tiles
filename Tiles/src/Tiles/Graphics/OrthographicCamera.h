#pragma once

#include <glm/glm.hpp>

namespace Tiles
{
	/// 2D orthographic camera: pan (position/move), zoom, and a Y-flipped ortho
	/// projection matching the top-left screen origin. Owns its view and
	/// projection matrices and converts screen pixels to world space.
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left = -10.0f, float right = 10.0f, float bottom = -10.0f, float top = 10.0f, float nearPlane = -100.0f, float farPlane = 100.0f);

		void SetPosition(const glm::vec3& position);
		const glm::vec3& GetPosition() const { return m_Position; }

		void MoveRight(float distance);
		void MoveUp(float distance);

		/// Orients the camera toward a target point (a no-op for the top-down 2D
		/// view, but kept for callers that frame content explicitly).
		void LookAt(const glm::vec3& target);

		void SetBounds(float left, float right, float bottom, float top);
		void SetClippingPlanes(float nearPlane, float farPlane);
		/// Sets the base (unzoomed) view size and recomputes bounds around center.
		void SetSize(float width, float height);
		/// Scales the base view size by 1/@p zoom about the current center, so
		/// higher zoom shows a smaller area. Clamped to a small positive value.
		void SetZoom(float zoom);
		void SetOrthoParams(float left, float right, float bottom, float top, float nearPlane, float farPlane);
		float GetZoom() const { return m_Zoom; }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

		/// Maps a screen-space pixel (within a viewport of screenSize) to world space.
		glm::vec3 ScreenToWorld(const glm::vec2& screenPos, const glm::vec2& screenSize, float depth = 0.0f) const;

	private:
		glm::vec3 GetForward() const;
		glm::vec3 GetRight() const;
		glm::vec3 GetUp() const;

		void UpdateViewMatrix();
		void UpdateProjectionMatrix();

	private:
		glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec3 m_Rotation = glm::vec3(0.0f);
		glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);

		float m_Left;
		float m_Right;
		float m_Bottom;
		float m_Top;
		float m_NearPlane;
		float m_FarPlane;

		float m_Zoom;
		float m_BaseWidth;
		float m_BaseHeight;
	};
}
