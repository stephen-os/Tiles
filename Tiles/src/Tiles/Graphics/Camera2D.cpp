#include "Camera2D.h"

#include <algorithm>

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

	// Returns the camera to the world origin at default zoom.
	void Camera2D::Reset()
	{
		Center = { 0.0f, 0.0f };
		Zoom = 1.0f;
	}

	// Frames the content's tile bounds within a nominal viewport, or sits at the
	// origin when there is nothing to frame.
	void Camera2D::Fit(const std::optional<glm::ivec4>& bounds, float tileSize, float minZoom, float maxZoom)
	{
		// With nothing painted there is no content to frame; sit at the origin.
		if (!bounds)
		{
			Reset();
			return;
		}

		const float minX = static_cast<float>(bounds->x);
		const float minY = static_cast<float>(bounds->y);
		const float maxX = static_cast<float>(bounds->z);
		const float maxY = static_cast<float>(bounds->w);

		// A tile at coord (cx, cy) fills the cell [cx, cx+1], so the content spans
		// [minX, maxX+1] in tiles; center on the midpoint in world units.
		Center = {
			(minX + maxX + 1.0f) * 0.5f * tileSize,
			(minY + maxY + 1.0f) * 0.5f * tileSize
		};

		const float contentWidth = (maxX - minX + 1.0f) * tileSize;
		const float contentHeight = (maxY - minY + 1.0f) * tileSize;

		// Assumes a nominal viewport size; the 0.9 factor leaves a margin so the
		// content does not sit flush against the edges.
		const float viewportWidth = 800.0f;
		const float viewportHeight = 600.0f;

		const float zoomX = viewportWidth / contentWidth;
		const float zoomY = viewportHeight / contentHeight;
		const float fitZoom = std::min(zoomX, zoomY) * 0.9f;

		Zoom = std::clamp(fitZoom, minZoom, maxZoom);
	}
}
