#include "ViewportCameraController.h"

#include <algorithm>

#include "../Constants.h"

namespace Tiles
{
    ViewportCameraController::ViewportCameraController() {}

    void ViewportCameraController::Initialize()
    {
        m_Camera.Center = { 0.0f, 0.0f };
        m_Camera.Zoom = 1.0f;
    }

    void ViewportCameraController::Center()
    {
        m_Camera.Center = { 0.0f, 0.0f };
    }

    void ViewportCameraController::Fit(const std::optional<glm::ivec4>& bounds)
    {
        // With nothing painted there is no content to frame; sit at the origin.
        if (!bounds)
        {
            Initialize();
            return;
        }

        const float tileSize = Viewport::Render::DefaultTileSize;

        // A tile at coord (cx, cy) is centered at world (cx * tileSize, cy * tileSize),
        // so the painted content spans one extra tile in each dimension.
        const float minX = bounds->x, minY = bounds->y;
        const float maxX = bounds->z, maxY = bounds->w;

        m_Camera.Center = {
            (minX + maxX) * 0.5f * tileSize,
            (minY + maxY) * 0.5f * tileSize
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

        m_Camera.Zoom = std::clamp(fitZoom, Viewport::Render::MinZoom, Viewport::Render::MaxZoom);
    }
}
