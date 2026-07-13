#pragma once

#include <optional>

#include <glm/glm.hpp>

namespace Tiles
{
	// A 2D pan-and-zoom view. The camera is fully described by the world point at
	// the centre of the viewport and a zoom (pixels per world unit). The
	// view-projection matrix and screen<->world mapping are derived on demand from
	// those plus the viewport size -- there is no rotation and no cached matrix.
	struct Camera2D
	{
		glm::vec2 Center{ 0.0f };   // world point at the centre of the viewport
		float     Zoom = 1.0f;      // pixels per world unit (higher = more zoomed in)

		// Y-flipped orthographic view-projection for a viewport of @p viewportSize
		// pixels (top-left origin, +Y down). The depth range is fixed so per-layer
		// depth offsets keep sorting tiles/grid.
		glm::mat4 ViewProjection(const glm::vec2& viewportSize) const;

		// Maps a screen pixel (top-left origin) within @p viewportSize to a
		// world-space point.
		glm::vec2 ScreenToWorld(const glm::vec2& pixel, const glm::vec2& viewportSize) const;

		// Returns the camera to the world origin at default zoom.
		void Reset();

		// Centers and zooms so the content's inclusive tile @p bounds fit a nominal
		// viewport, clamped to [minZoom, maxZoom]; resets to the origin when @p
		// bounds is empty. @p tileSize is the world size of one tile.
		void Fit(const std::optional<glm::ivec4>& bounds, float tileSize, float minZoom, float maxZoom);
	};
}
