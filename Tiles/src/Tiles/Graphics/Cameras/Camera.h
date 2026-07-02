// Camera.h
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace Tiles
{

	class Camera
	{
	public:
		struct Ray
		{
			glm::vec3 Origin;
			glm::vec3 Direction;
		};

		struct Frustum
		{
			glm::vec4 Left;
			glm::vec4 Right;
			glm::vec4 Bottom;
			glm::vec4 Top;
			glm::vec4 Near;
			glm::vec4 Far;
		};

	public:
		Camera();
		virtual ~Camera() = default;

		virtual void SetPosition(const glm::vec3& position) = 0;
		/// Moves the camera in world space.
		void Translate(const glm::vec3& translation);
		/// Moves the camera along its own right/up/forward axes.
		void TranslateLocal(const glm::vec3& translation);

		void SetRotation(const glm::vec3& rotation);
		void SetEulerAngles(float pitch, float yaw, float roll);
		void Rotate(const glm::vec3& rotation);
		void RotatePitch(float angle);
		void RotateYaw(float angle);
		void RotateRoll(float angle);

		/// Orients the camera to face @p target, deriving pitch/yaw and keeping roll.
		void LookAt(const glm::vec3& target);
		/// Orients toward @p target using @p up to also resolve roll.
		void LookAt(const glm::vec3& target, const glm::vec3& up);
		/// Like LookAt but interpolates toward the target orientation by
		/// @p speed * @p deltaTime; call each frame to animate.
		void LookAtSmooth(const glm::vec3& target, float speed, float deltaTime);
		void LookAtSmooth(const glm::vec3& target, float speed, float deltaTime, const glm::vec3& up);

		const glm::vec3& GetPosition() const { return m_Position; }
		const glm::vec3& GetRotation() const { return m_Rotation; }

		glm::vec3 GetForward() const;
		glm::vec3 GetRight() const;
		glm::vec3 GetUp() const;
		float GetPitch() const { return m_Rotation.x; }
		float GetYaw() const { return m_Rotation.y; }
		float GetRoll() const { return m_Rotation.z; }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; };
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; };
		glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

		void SetPitchConstraints(float minPitch, float maxPitch);
		void SetYawConstraints(float minYaw, float maxYaw);
		void SetRollConstraints(float minRoll, float maxRoll);
		void ClearConstraints();

		// Movement helpers
		void MoveForward(float distance);
		void MoveRight(float distance);
		void MoveUp(float distance);

		/// Unprojects a pixel position to a world-space point at the given NDC
		/// @p depth (-1 near, +1 far). @p screenSize is the viewport in pixels.
		glm::vec3 ScreenToWorld(const glm::vec2& screenPos, const glm::vec2& screenSize, float depth = 0.0f) const;
		/// Projects a world-space point to a pixel position within @p screenSize.
		glm::vec2 WorldToScreen(const glm::vec3& worldPos, const glm::vec2& screenSize) const;

		/// Builds a world-space ray from the near to far plane through @p screenPos
		/// (e.g. for mouse picking).
		Ray GetRay(const glm::vec2& screenPos, const glm::vec2& screenSize) const;

		/// Extracts the six frustum planes from the view-projection matrix, each
		/// pointing inward so a positive plane distance means inside.
		Frustum GetFrustum() const;
		bool IsPointInFrustum(const glm::vec3& point) const;
		bool IsSphereInFrustum(const glm::vec3& center, float radius) const;

		void SetTargetPosition(const glm::vec3& target);
		void SetTargetRotation(const glm::vec3& target);
		/// Advances any set position/rotation targets toward them by an amount
		/// proportional to @p deltaTime; clears a target once it is reached.
		void UpdateSmooth(float deltaTime, float positionSpeed = 5.0f, float rotationSpeed = 5.0f);

	protected:
		void UpdateViewMatrix();
		virtual void UpdateProjectionMatrix() = 0;

		void ApplyConstraints();

	protected:
		// Core
		glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec3 m_Rotation = glm::vec3(0.0f);
		glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

		// Matrices
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);

		// Constraints
		bool m_HasPitchConstraints = false;
		bool m_HasYawConstraints = false;
		bool m_HasRollConstraints = false;
		glm::vec2 m_PitchConstraints = glm::vec2(-90.0f, 90.0f);
		glm::vec2 m_YawConstraints = glm::vec2(-180.0f, 180.0f);
		glm::vec2 m_RollConstraints = glm::vec2(-180.0f, 180.0f);

		// Smooth animation targets
		bool m_HasPositionTarget = false;
		bool m_HasRotationTarget = false;
		glm::vec3 m_TargetPosition = glm::vec3(0.0f);
		glm::vec3 m_TargetRotation = glm::vec3(0.0f);
	};
}
