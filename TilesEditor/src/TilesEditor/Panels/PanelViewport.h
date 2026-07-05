#pragma once
#include <memory>

#include "Panel.h"
#include "Tiles.h"
#include "imgui.h"



namespace Tiles::Editor
{
    /// The main editing canvas. Draws the grid, layers, and a hover/brush preview
    /// into an offscreen RenderTarget each frame and blits it via ImGui::Image.
    /// Paint and camera input are only processed while the window is focused, so
    /// Render() builds the frame and Update() handles interaction.
    class PanelViewport : public Panel
    {
    public:
        PanelViewport(std::shared_ptr<Context> context);
        ~PanelViewport() = default;

        void Render() override;
        void Update() override;

    private:
        /// Maps the current mouse position from screen space into world space,
        /// accounting for the viewport origin, its centered projection, and zoom.
        glm::vec2 ScreenToWorld() const;

        /// Returns the signed tile coord under the mouse (the cell containing the
        /// world point). Any coord (including negative) is paintable.
        glm::ivec2 GetGridPositionUnderMouse() const;

        void RenderGrid();
        /// Draws red X/Y axes through the world origin, spanning the visible view.
        void RenderOrigin();
        /// Draws the project's export-region rectangle when it is enabled.
        void RenderExportRegion();
        void RenderLayers();
        void RenderLayer(const TileLayer& layer, size_t layerIndex);
        void RenderHoverTile();

        /// ImGui overlay drawn over the blitted viewport: origin/fit buttons and
        /// the hovered tile coordinate. Sets m_PointerOverOverlay.
        void RenderOverlay();
        void RenderBrushPreview(const Tile& brush);
        void RenderEraserPreview();
        void RenderFillPreview();
        void RenderBasicHover();

        void ExecutePaintAction(const glm::ivec2& gridPos);

        void HandleInput();
        void HandleMouseDragging(); 
        void HandleCameraMovement();
        void HandleZoom();

    private:
        std::shared_ptr<RenderTarget> m_RenderTarget;

        float m_TileSize;
        float m_MouseDelta = 0.0f;
        bool m_IsDragging = false;
        bool m_IsWindowFocused = false;
        bool m_PointerOverOverlay = false;   // pointer is over the overlay controls; suppresses painting

        ImVec2 m_CurrentMousePosition = { 0.0f, 0.0f };
        ImVec2 m_PreviousMousePosition = { 0, 0 };
        ImVec2 m_ViewportPosition = { 0.0f, 0.0f };
        ImVec2 m_ViewportSize = { 512.0f, 512.0f };

        glm::vec3 m_MouseFollowQuadPosition = { 0.0f, 0.0f, 0.2f };
        glm::vec2 m_MouseFollowQuadSize = { 32.0f, 32.0f };
        glm::vec4 m_MouseFollowQuadColor = { 0.0f, 1.0f, 0.0f, 0.6f };
    };
}