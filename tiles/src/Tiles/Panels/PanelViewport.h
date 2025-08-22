#pragma once

#include "Panel.h"
#include "Lumina/Lumina.h"
#include "imgui.h"

using namespace Lumina;

namespace Tiles
{
    class PanelViewport : public Panel
    {
    public:
        PanelViewport();
        ~PanelViewport() = default;

        void Render() override;
        void Update() override;

    private:
        // Coordinate transformation
        glm::vec2 ScreenToWorld() const;
        glm::ivec2 GetGridPositionUnderMouse() const;
        bool IsValidGridPosition(const glm::ivec2& gridPos) const;

        // Rendering methods
        void RenderGrid();
        void RenderLayerBoundaries();
        void RenderLayers();
        void RenderLayer(const TileLayer& layer, size_t layerIndex, const glm::vec3& cameraPos);
        void RenderHoverTile();
        void RenderBrushPreview(const Tile& brush, const glm::vec3& cameraPos);
        void RenderEraserPreview();
        void RenderFillPreview();
        void RenderBasicHover();

        // Input handling
        void HandleInput();
        void ExecutePaintAction(const glm::ivec2& gridPos);

    private:
        Ref<OrthographicCamera> m_Camera;
        Ref<RenderTarget> m_RenderTarget;

        float m_TileSize;
        float m_MouseDelta = 0.0f;

        ImVec2 m_MousePosition = { 0.0f, 0.0f };
        ImVec2 m_ViewportPosition = { 0.0f, 0.0f };
        ImVec2 m_ViewportSize = { 512.0f, 512.0f };

        glm::vec3 m_MouseFollowQuadPosition = { 0.0f, 0.0f, 0.2f };
        glm::vec2 m_MouseFollowQuadSize = { 32.0f, 32.0f };
        glm::vec4 m_MouseFollowQuadColor = { 0.0f, 1.0f, 0.0f, 0.6f };
    };
}