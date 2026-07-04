#pragma once

#include <optional>

#include <glm/glm.hpp>

#include "../../Graphics/Camera2D.h"

namespace Tiles
{
    /// Owns the viewport camera and the framing math that positions it on the
    /// unbounded board. The world origin (0, 0) is the reference point; painted
    /// content is framed via its bounding box, so the controller has no dependency
    /// on Project or the rest of the editor state.
    class ViewportCameraController
    {
    public:
        ViewportCameraController();

        Camera2D& GetCamera() { return m_Camera; }
        const Camera2D& GetCamera() const { return m_Camera; }

        /// Centers the camera on the world origin at default zoom.
        void Initialize();
        /// Centers the camera on the world origin, preserving the current zoom.
        void Center();
        /// Centers and zooms so the painted content's bounding box fits within a
        /// nominal viewport; falls back to the origin at default zoom if empty.
        /// @param bounds Inclusive tile bounds (minX, minY, maxX, maxY), or nullopt.
        void Fit(const std::optional<glm::ivec4>& bounds);

    private:
        Camera2D m_Camera;
    };
}
