#include "ViewportCameraController.h"

#include <algorithm>

#include "../Constants.h"

namespace Tiles
{
    ViewportCameraController::ViewportCameraController() {}

    void ViewportCameraController::Initialize(uint32_t gridWidth, uint32_t gridHeight)
    {
        m_Camera.Center = {
            Viewport::Render::DefaultTileSize * (static_cast<float>(gridWidth) * 0.5f),
            Viewport::Render::DefaultTileSize * (static_cast<float>(gridHeight) * 0.5f)
        };

        m_Camera.Zoom = 1.0f;
    }

    void ViewportCameraController::Center(uint32_t gridWidth, uint32_t gridHeight)
    {
        m_Camera.Center = {
            Viewport::Render::DefaultTileSize * (static_cast<float>(gridWidth) * 0.5f),
            Viewport::Render::DefaultTileSize * (static_cast<float>(gridHeight) * 0.5f)
        };
    }

    void ViewportCameraController::Fit(uint32_t gridWidth, uint32_t gridHeight)
    {
        const float projectWidth = gridWidth * Viewport::Render::DefaultTileSize;
        const float projectHeight = gridHeight * Viewport::Render::DefaultTileSize;

        Center(gridWidth, gridHeight);

        // Assumes a nominal viewport size; the 0.9 factor leaves a margin so the
        // grid does not sit flush against the edges.
        const float viewportWidth = 800.0f;
        const float viewportHeight = 600.0f;

        const float zoomX = viewportWidth / projectWidth;
        const float zoomY = viewportHeight / projectHeight;
        const float fitZoom = std::min(zoomX, zoomY) * 0.9f;

        const float clampedZoom = std::clamp(fitZoom, Viewport::Render::MinZoom, Viewport::Render::MaxZoom);
        m_Camera.Zoom = clampedZoom;
    }

    void ViewportCameraController::FollowResize(uint32_t oldWidth, uint32_t oldHeight, uint32_t newWidth, uint32_t newHeight)
    {
        const float relativeX = m_Camera.Center.x / (static_cast<float>(oldWidth) * Viewport::Render::DefaultTileSize);
        const float relativeY = m_Camera.Center.y / (static_cast<float>(oldHeight) * Viewport::Render::DefaultTileSize);

        const float clampedX = std::clamp(relativeX, 0.0f, 1.0f);
        const float clampedY = std::clamp(relativeY, 0.0f, 1.0f);

        m_Camera.Center = {
            clampedX * newWidth * Viewport::Render::DefaultTileSize,
            clampedY * newHeight * Viewport::Render::DefaultTileSize
        };
    }
}
