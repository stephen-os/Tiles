#pragma once

#include <cstdint>
#include <memory>

#include "../../Graphics/Camera2D.h"

namespace Tiles
{
    /// Owns the viewport camera and the framing math that positions it relative
    /// to the project grid. Grid dimensions are passed in, so the controller has
    /// no dependency on Project or the rest of the editor state.
    class ViewportCameraController
    {
    public:
        ViewportCameraController();

        Camera2D& GetCamera() { return m_Camera; }
        const Camera2D& GetCamera() const { return m_Camera; }

        /// Centers the camera on the grid at default zoom.
        void Initialize(uint32_t gridWidth, uint32_t gridHeight);
        /// Centers the camera on the grid, preserving the current depth/zoom.
        void Center(uint32_t gridWidth, uint32_t gridHeight);
        /// Centers and zooms so the whole grid fits within a nominal viewport.
        void Fit(uint32_t gridWidth, uint32_t gridHeight);
        /// Keeps the camera focused on the same relative point after a resize.
        void FollowResize(uint32_t oldWidth, uint32_t oldHeight, uint32_t newWidth, uint32_t newHeight);

    private:
        Camera2D m_Camera;
    };
}
