#include "ViewportCameraController.h"

#include <algorithm>

namespace Tiles
{
	namespace
	{
		// Camera framing limits used when fitting content to the viewport.
		constexpr float DefaultTileSize = 64.0f;
		constexpr float MinZoom = 0.5f;
		constexpr float MaxZoom = 3.0f;
	}

	ViewportCameraController::ViewportCameraController() {}

	// Centers the camera on the world origin at default zoom.
	void ViewportCameraController::Initialize()
	{
		m_Camera.Center = { 0.0f, 0.0f };
		m_Camera.Zoom = 1.0f;
	}

	// Centers the camera on the world origin, preserving the current zoom.
	void ViewportCameraController::Center()
	{
		m_Camera.Center = { 0.0f, 0.0f };
	}

	// Frames the painted content's bounding box within a nominal viewport, or
	// sits at the origin when there is nothing painted.
	void ViewportCameraController::Fit(const std::optional<glm::ivec4>& bounds)
	{
		// With nothing painted there is no content to frame; sit at the origin.
		if (!bounds)
		{
			Initialize();
			return;
		}

		const float tileSize = DefaultTileSize;

		// A tile at coord (cx, cy) fills the cell [cx, cx+1], centered at
		// (cx + 0.5) * tileSize, so the content spans [minX, maxX+1] in tiles.
		const float minX = bounds->x, minY = bounds->y;
		const float maxX = bounds->z, maxY = bounds->w;

		m_Camera.Center = {
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

		m_Camera.Zoom = std::clamp(fitZoom, MinZoom, MaxZoom);
	}
}
