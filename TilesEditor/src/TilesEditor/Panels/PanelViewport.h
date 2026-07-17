#pragma once
#include <memory>
#include <vector>

#include "Panel.h"
#include "Tiles.h"
#include "imgui.h"



namespace Tiles::Editor
{
    // The main editing canvas. Draws the grid, layers, and a hover/brush preview
    // into an offscreen RenderTarget each frame and blits it via ImGui::Image.
    // Paint and camera input are only processed while the window is focused, so
    // Render() builds the frame and Update() handles interaction.
    class PanelViewport : public Panel
    {
    public:
        PanelViewport(EditorHost& host);
        ~PanelViewport() = default;

        void Render() override;
        void Update() override;

    private:
        // Maps the current mouse position from screen space into world space,
        // accounting for the viewport origin, its centered projection, and zoom.
        glm::vec2 ScreenToWorld() const;

        // Returns the signed tile coord under the mouse (the cell containing the
        // world point). Any coord (including negative) is paintable.
        glm::ivec2 GetGridPositionUnderMouse() const;

        void RenderGrid();
        // Draws red X/Y axes through the world origin, spanning the visible view.
        void RenderOrigin();
        // Draws the project's export-region rectangle when it is enabled.
        void RenderExportRegion();
        void RenderLayers();
        void RenderLayer(const TileLayer& layer, size_t layerIndex);
        // Previews the pending edit: the accumulated stroke while dragging, else
        // the brush footprint under the cursor.
        void RenderHoverTile();
        // Draws the current mode's preview quad (brush/eraser/fill) at one cell.
        void RenderCellPreview(const glm::ivec2& cell);

        // ImGui overlay drawn over the blitted viewport: origin/fit buttons and
        // the hovered tile coordinate. Sets m_PointerOverOverlay.
        void RenderOverlay();

        void HandleInput();
        // Paint-stroke lifecycle (Brush/Eraser): begin on press, extend across the
        // drag, commit one command on release.
        void BeginStroke(const glm::ivec2& cell);
        void ExtendStroke(const glm::ivec2& cell);
        void CommitStroke();
        // Shape lifecycle (Line/Rectangle/Ellipse): anchor on press, recompute the
        // shape out to the cursor each drag frame, commit via CommitStroke on release.
        void BeginShape(const glm::ivec2& cell);
        void UpdateShape(const glm::ivec2& cell);
        // Adds the brush footprint centered on cell to the in-progress stroke.
        void AddFootprint(const glm::ivec2& cell);
        // Flood-fills from cell, bounded by the visible view.
        void FillAt(const glm::ivec2& cell);

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

        // In-progress paint stroke (Brush/Eraser): the cells accumulated while the
        // mouse is held, committed as one command on release.
        bool m_Stroking = false;
        glm::ivec2 m_LastStrokeCell = { 0, 0 };
        glm::ivec2 m_ShapeAnchor = { 0, 0 };
        std::vector<glm::ivec2> m_StrokeCells;
    };
}