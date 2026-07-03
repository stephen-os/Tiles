#include "Camera2D.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Tiles
{
	// Depth range shared by every 2D view; wide enough to hold the small per-layer
	// depth offsets used to order tiles and keep the grid behind them.
	static constexpr float s_NearClip = -100.0f;
	static constexpr float s_FarClip = 100.0f;

	glm::mat4 Camera2D::ViewProjection(const glm::vec2& viewportSize) const
	{
		float halfWidth = viewportSize.x / Zoom * 0.5f;
		float halfHeight = viewportSize.y / Zoom * 0.5f;

		// Top and bottom are swapped to flip Y, so +Y points down and world space
		// matches the top-left screen origin.
		return glm::ortho(
			Center.x - halfWidth, Center.x + halfWidth,
			Center.y + halfHeight, Center.y - halfHeight,
			s_NearClip, s_FarClip);
	}

	glm::vec2 Camera2D::ScreenToWorld(const glm::vec2& pixel, const glm::vec2& viewportSize) const
	{
		float halfWidth = viewportSize.x / Zoom * 0.5f;
		float halfHeight = viewportSize.y / Zoom * 0.5f;

		// Pixel (top-left origin) -> normalized device coordinates -> world.
		float ndcX = 2.0f * pixel.x / viewportSize.x - 1.0f;
		float ndcY = 1.0f - 2.0f * pixel.y / viewportSize.y;

		return { Center.x + ndcX * halfWidth, Center.y - ndcY * halfHeight };
	}
}
