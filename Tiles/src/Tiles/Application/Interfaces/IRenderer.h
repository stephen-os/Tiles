#pragma once

#include "../../Domain/Entities/TileProject.h"
#include "../../Domain/Entities/TileGrid.h"

#include <glm/glm.hpp>
#include <memory>

namespace Tiles::Services
{
    // Forward declarations
    class ITexture;
    class ICamera;

    // Interface for 2D rendering operations
    // Implemented by Infrastructure layer (OpenGL, Vulkan, etc.)
    class IRenderer
    {
    public:
        virtual ~IRenderer() = default;

        // Lifecycle
        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;

        // Frame management
        virtual void BeginFrame(const ICamera& camera) = 0;
        virtual void EndFrame() = 0;

        // Primitive drawing
        virtual void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) = 0;
        virtual void DrawTexturedQuad(const glm::vec3& position, const glm::vec2& size,
                                       const ITexture& texture, const glm::vec4& texCoords = glm::vec4(0, 0, 1, 1),
                                       const glm::vec4& tintColor = glm::vec4(1)) = 0;
        virtual void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float thickness = 1.0f) = 0;

        // Grid drawing
        virtual void DrawGrid(const glm::vec3& position, const glm::vec2& size, float cellSize, const glm::vec4& color) = 0;

        // Resolution
        virtual void SetResolution(uint32_t width, uint32_t height) = 0;
        virtual glm::vec2 GetResolution() const = 0;
    };

    // Interface for textures
    class ITexture
    {
    public:
        virtual ~ITexture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetID() const = 0;
        virtual void Bind(uint32_t slot = 0) const = 0;
    };

    // Interface for cameras
    class ICamera
    {
    public:
        virtual ~ICamera() = default;

        virtual glm::mat4 GetViewMatrix() const = 0;
        virtual glm::mat4 GetProjectionMatrix() const = 0;
        virtual glm::mat4 GetViewProjectionMatrix() const = 0;

        virtual void SetPosition(const glm::vec3& position) = 0;
        virtual glm::vec3 GetPosition() const = 0;

        virtual void SetZoom(float zoom) = 0;
        virtual float GetZoom() const = 0;
    };
}
