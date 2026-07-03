#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace Tiles
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float nearPlane, float farPlane)
		: m_Left(left), m_Right(right), m_Bottom(bottom), m_Top(top), m_NearPlane(nearPlane), m_FarPlane(farPlane)
	{
		m_BaseWidth = right - left;
		m_BaseHeight = top - bottom;
		m_Zoom = 1.0f;

		UpdateViewMatrix();
		UpdateProjectionMatrix();
	}

	void OrthographicCamera::SetPosition(const glm::vec3& position)
	{
		glm::vec3 delta = position - m_Position;
		m_Position = position;

		m_Left += delta.x;
		m_Right += delta.x;
		m_Bottom += delta.y;
		m_Top += delta.y;

		UpdateViewMatrix();
		UpdateProjectionMatrix();
	}

	void OrthographicCamera::MoveRight(float distance)
	{
		m_Position += GetRight() * distance;
		UpdateViewMatrix();
	}

	void OrthographicCamera::MoveUp(float distance)
	{
		m_Position += GetUp() * distance;
		UpdateViewMatrix();
	}

	void OrthographicCamera::LookAt(const glm::vec3& target)
	{
		glm::vec3 direction = glm::normalize(m_Position - target);

		// Yaw/pitch to face the target; roll is left unchanged.
		m_Rotation.y = glm::degrees(atan2(direction.x, direction.z));
		m_Rotation.x = glm::degrees(asin(-direction.y));

		UpdateViewMatrix();
	}

	void OrthographicCamera::SetBounds(float left, float right, float bottom, float top)
	{
		m_Left = left;
		m_Right = right;
		m_Bottom = bottom;
		m_Top = top;

		m_BaseWidth = right - left;
		m_BaseHeight = top - bottom;

		SetZoom(m_Zoom);

		UpdateProjectionMatrix();
	}

	void OrthographicCamera::SetClippingPlanes(float nearPlane, float farPlane)
	{
		m_NearPlane = nearPlane;
		m_FarPlane = farPlane;

		UpdateProjectionMatrix();
	}

	void OrthographicCamera::SetSize(float width, float height)
	{
		m_BaseWidth = width;
		m_BaseHeight = height;

		float newWidth = m_BaseWidth / m_Zoom;
		float newHeight = m_BaseHeight / m_Zoom;
		float halfWidth = newWidth / 2.0f;
		float halfHeight = newHeight / 2.0f;

		glm::vec3 center = GetPosition();

		m_Left = center.x - halfWidth;
		m_Right = center.x + halfWidth;
		m_Bottom = center.y - halfHeight;
		m_Top = center.y + halfHeight;

		UpdateProjectionMatrix();
	}

	void OrthographicCamera::SetZoom(float zoom)
	{
		if (zoom <= 0.0f)
			zoom = 0.001f;
		m_Zoom = zoom;

		float newWidth = m_BaseWidth / zoom;
		float newHeight = m_BaseHeight / zoom;
		float halfWidth = newWidth / 2.0f;
		float halfHeight = newHeight / 2.0f;

		glm::vec3 center = GetPosition();

		m_Left = center.x - halfWidth;
		m_Right = center.x + halfWidth;
		m_Bottom = center.y - halfHeight;
		m_Top = center.y + halfHeight;

		UpdateProjectionMatrix();
	}

	void OrthographicCamera::SetOrthoParams(float left, float right, float bottom, float top, float nearPlane, float farPlane)
	{
		m_Left = left;
		m_Right = right;
		m_Bottom = bottom;
		m_Top = top;
		m_NearPlane = nearPlane;
		m_FarPlane = farPlane;

		m_BaseWidth = right - left;
		m_BaseHeight = top - bottom;

		SetZoom(m_Zoom);

		UpdateProjectionMatrix();
	}

	glm::vec3 OrthographicCamera::ScreenToWorld(const glm::vec2& screenPos, const glm::vec2& screenSize, float depth) const
	{
		// Screen pixel -> normalized device coordinates.
		glm::vec4 ndc;
		ndc.x = (2.0f * screenPos.x) / screenSize.x - 1.0f;
		ndc.y = 1.0f - (2.0f * screenPos.y) / screenSize.y;
		ndc.z = depth;
		ndc.w = 1.0f;

		glm::mat4 invViewProj = glm::inverse(GetViewProjectionMatrix());
		glm::vec4 worldPos = invViewProj * ndc;

		return glm::vec3(worldPos) / worldPos.w;
	}

	glm::vec3 OrthographicCamera::GetForward() const
	{
		float pitch = glm::radians(m_Rotation.x);
		float yaw = glm::radians(m_Rotation.y);

		return glm::normalize(glm::vec3(
			cos(pitch) * sin(yaw),
			-sin(pitch),
			cos(pitch) * cos(yaw)
		));
	}

	glm::vec3 OrthographicCamera::GetRight() const
	{
		glm::vec3 forward = GetForward();
		return glm::normalize(glm::cross(forward, m_Up));
	}

	glm::vec3 OrthographicCamera::GetUp() const
	{
		glm::vec3 forward = GetForward();
		glm::vec3 right = GetRight();
		return glm::normalize(glm::cross(right, forward));
	}

	void OrthographicCamera::UpdateViewMatrix()
	{
		// Rotation matrix from Euler angles (YXZ order).
		glm::mat4 rotation = glm::mat4(1.0f);
		rotation = glm::rotate(rotation, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0)); // Yaw
		rotation = glm::rotate(rotation, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0)); // Pitch
		rotation = glm::rotate(rotation, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1)); // Roll

		// The view matrix is the inverse of the camera transform: transpose the
		// (orthogonal) rotation and negate the translation.
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), -m_Position);
		m_ViewMatrix = glm::transpose(rotation) * translation;
	}

	void OrthographicCamera::UpdateProjectionMatrix()
	{
		// Top and bottom are passed swapped to flip Y, so +Y points down and
		// world space matches the top-left origin screen convention.
		m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Top, m_Bottom, m_NearPlane, m_FarPlane);
	}
}
