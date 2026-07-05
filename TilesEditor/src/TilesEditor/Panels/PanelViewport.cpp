#include "PanelViewport.h"

#include "Core/Constants.h"
#include "../UIConstants.h"

#include "Core/Input.h"
#include "../Rendering/TileSceneRenderer.h"
#include <algorithm>
#include <cmath>

namespace Tiles::Editor
{
    PanelViewport::PanelViewport(std::shared_ptr<Context> context)
        : Panel(context), m_TileSize(Viewport::Render::DefaultTileSize)
    {
        m_RenderTarget = Tiles::RenderTarget::Create(512, 512);
        m_MouseFollowQuadSize = { m_TileSize * 0.5f, m_TileSize * 0.5f };
        m_MouseFollowQuadColor = Viewport::Grid::HoverColor;
    }

    void PanelViewport::Render()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
        ImGui::Begin("Viewport", nullptr, flags);

		m_IsWindowFocused = ImGui::IsWindowFocused();

        if (!m_Context || !m_Context->HasProject())
        {
            ImGui::TextColored(UI::Color::TextError, "No project loaded");
            ImGui::End();
            return;
        }

        Camera2D& camera = m_Context->GetViewportCamera();

        m_CurrentMousePosition = ImGui::GetMousePos();
        m_ViewportPosition = ImGui::GetCursorScreenPos();
        m_ViewportSize = ImGui::GetContentRegionAvail();
        m_ViewportSize.x = std::max(m_ViewportSize.x, m_TileSize);
        m_ViewportSize.y = std::max(m_ViewportSize.y, m_TileSize);

        Tiles::Renderer2D::SetRenderTarget(m_RenderTarget);
        Tiles::Renderer2D::SetResolution(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
        Tiles::Renderer2D::BeginFrame(camera.ViewProjection({ m_ViewportSize.x, m_ViewportSize.y }));

        RenderGrid();
        RenderLayers();
        RenderOrigin();
        RenderExportRegion();
        RenderHoverTile();

        Tiles::Renderer2D::EndFrame();
        Tiles::Renderer2D::SetRenderTarget(nullptr);

        ImGui::Image((void*)m_RenderTarget->GetTexture(), m_ViewportSize);

        RenderOverlay();

        m_MouseDelta = ImGui::GetIO().MouseWheel;

        ImGui::End();
    }

    void PanelViewport::Update()
    {
        if (!m_Context || !m_Context->HasProject())
            return;

        if (m_IsWindowFocused)
        {
            HandleInput();
            HandleZoom();
            HandleCameraMovement();
            HandleMouseDragging();
        }
    }

    void PanelViewport::HandleCameraMovement()
    {
        // A held modifier means a menu shortcut (e.g. Ctrl+E to export), not
        // camera input -- don't pan/zoom underneath it.
        if (ImGui::GetIO().KeyCtrl || ImGui::GetIO().KeyAlt || ImGui::GetIO().KeySuper)
            return;

        Camera2D& camera = m_Context->GetViewportCamera();

        if (Input::IsKeyPressed(KeyCode::W))
            camera.Center.y += Viewport::Input::CameraMoveSpeed;
        if (Input::IsKeyPressed(KeyCode::S))
            camera.Center.y -= Viewport::Input::CameraMoveSpeed;
        if (Input::IsKeyPressed(KeyCode::A))
            camera.Center.x -= Viewport::Input::CameraMoveSpeed;
        if (Input::IsKeyPressed(KeyCode::D))
            camera.Center.x += Viewport::Input::CameraMoveSpeed;

        if (Input::IsKeyPressed(KeyCode::Q))
            camera.Zoom = std::min(camera.Zoom * 1.02f, Viewport::Render::MaxZoom);
        if (Input::IsKeyPressed(KeyCode::E))
            camera.Zoom = std::max(camera.Zoom / 1.02f, Viewport::Render::MinZoom);
    }

    void PanelViewport::HandleZoom()
    {
        Camera2D& camera = m_Context->GetViewportCamera();
        if (m_MouseDelta == 0) return;

        camera.Zoom = std::clamp(
            camera.Zoom + m_MouseDelta * Viewport::Render::ZoomSensitivity,
            Viewport::Render::MinZoom,
            Viewport::Render::MaxZoom
        );
    }

    void PanelViewport::HandleMouseDragging()
    {
        Camera2D& camera = m_Context->GetViewportCamera();

        // A pan begins on middle-button press and ends only when middle is
        // released; while dragging, either middle or right may hold the pan.
        // World Y grows upward, so the vertical delta is added rather than subtracted.
        if (Input::IsMouseButtonPressed(MouseCode::Middle) && !m_IsDragging)
        {
            m_IsDragging = true;
            m_PreviousMousePosition = m_CurrentMousePosition;
        }

        if (m_IsDragging && (Input::IsMouseButtonPressed(MouseCode::Right) || Input::IsMouseButtonPressed(MouseCode::Middle)))
        {
            ImVec2 mouseDelta = {
                m_CurrentMousePosition.x - m_PreviousMousePosition.x,
                m_CurrentMousePosition.y - m_PreviousMousePosition.y
            };

            float dragSensitivity = CameraConstants::Settings::DragSensitivity;
            float zoom = camera.Zoom;

            camera.Center.x -= mouseDelta.x * dragSensitivity / zoom;
            camera.Center.y += mouseDelta.y * dragSensitivity / zoom;

            m_PreviousMousePosition = m_CurrentMousePosition;
        }

        if (m_IsDragging && !Input::IsMouseButtonPressed(MouseCode::Middle))
        {
            m_IsDragging = false;
        }
    }

    void PanelViewport::RenderGrid()
    {
        // Colors come from GridParams' defaults; the grid reconstructs world
        // space from the frame's view-projection (set by BeginFrame). Grid lines
        // fall on integer tile boundaries (so they pass through the origin); a
        // tile at (cx, cy) fills the cell between them.
        Tiles::Renderer2D::DrawGrid({ .CellSize = m_TileSize });
    }

    void PanelViewport::RenderOrigin()
    {
        const Camera2D& camera = m_Context->GetViewportCamera();

        // Axes span the visible extent so they read as reference lines through the
        // world origin (only visible when the origin is in view).
        float halfWidth = m_ViewportSize.x / camera.Zoom * 0.5f;
        float halfHeight = m_ViewportSize.y / camera.Zoom * 0.5f;

        Tiles::LineParams line;
        line.Color = Viewport::Grid::BoundaryColor;   // red
        line.Thickness = 2.0f;

        line.Start = { camera.Center.x - halfWidth, 0.0f, Viewport::Depth::Outline };
        line.End = { camera.Center.x + halfWidth, 0.0f, Viewport::Depth::Outline };
        Tiles::Renderer2D::DrawLine(line);

        line.Start = { 0.0f, camera.Center.y - halfHeight, Viewport::Depth::Outline };
        line.End = { 0.0f, camera.Center.y + halfHeight, Viewport::Depth::Outline };
        Tiles::Renderer2D::DrawLine(line);
    }

    void PanelViewport::RenderExportRegion()
    {
        const ExportRegion& region = m_Context->GetProject()->GetExportRegion();
        if (!region.Enabled)
            return;

        // Region covers tiles [Min, Min+Size); a tile at (cx,cy) fills cell
        // [cx, cx+1], so the rectangle spans Min*tileSize to (Min+Size)*tileSize.
        float x0 = region.Min.x * m_TileSize;
        float y0 = region.Min.y * m_TileSize;
        float x1 = (region.Min.x + region.Size.x) * m_TileSize;
        float y1 = (region.Min.y + region.Size.y) * m_TileSize;

        Tiles::LineParams line;
        line.Color = { 0.2f, 0.8f, 1.0f, 1.0f };   // cyan, distinct from the red origin axes
        line.Thickness = 2.0f;

        auto edge = [&](float ax, float ay, float bx, float by)
        {
            line.Start = { ax, ay, Viewport::Depth::Outline };
            line.End = { bx, by, Viewport::Depth::Outline };
            Tiles::Renderer2D::DrawLine(line);
        };

        edge(x0, y0, x1, y0);
        edge(x1, y0, x1, y1);
        edge(x1, y1, x0, y1);
        edge(x0, y1, x0, y0);
    }

    void PanelViewport::RenderOverlay()
    {
        Camera2D& camera = m_Context->GetViewportCamera();

        ImGui::SetCursorScreenPos({ m_ViewportPosition.x + 8.0f, m_ViewportPosition.y + 8.0f });
        ImGui::BeginGroup();

        if (ImGui::Button("Origin"))
            camera.Center = { 0.0f, 0.0f };
        ImGui::SameLine();
        if (ImGui::Button("Fit"))
            m_Context->FitViewportCameraToProject();

        if (ImGui::IsWindowHovered())
        {
            glm::ivec2 coord = GetGridPositionUnderMouse();
            ImGui::Text("( %d, %d )", coord.x, coord.y);
        }

        ImGui::EndGroup();

        // Suppress painting while the pointer is over these controls, so clicking
        // a button doesn't also drop a tile underneath it.
        m_PointerOverOverlay = ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    }

    void PanelViewport::RenderLayers()
    {
        const LayerStack& layerStack = m_Context->GetProject()->GetLayerStack();

        for (size_t layerIdx = 0; layerIdx < layerStack.GetLayerCount(); ++layerIdx)
        {
            const TileLayer& layer = layerStack.GetLayer(layerIdx);
            if (!layer.GetVisibility()) continue;
            RenderLayer(layer, layerIdx);
        }
    }

    void PanelViewport::RenderLayer(const TileLayer& layer, size_t layerIndex)
    {
        const auto& textureAtlases = m_Context->GetProject()->GetTextureAtlases();
        DrawTileLayer(layer, layerIndex, m_TileSize, textureAtlases, Viewport::Depth::Tile);
    }

    void PanelViewport::RenderHoverTile()
    {
        if (!ImGui::IsWindowHovered()) return;

        glm::ivec2 gridPos = GetGridPositionUnderMouse();

        glm::vec2 gridCenter = {
            (gridPos.x + 0.5f) * m_TileSize,
            (gridPos.y + 0.5f) * m_TileSize
        };

        m_MouseFollowQuadPosition = {
            gridCenter.x,
            gridCenter.y,
            Viewport::Depth::HoverTile
        };

        const Tile& brush = m_Context->GetBrush();
        PaintingMode mode = m_Context->GetPaintingMode();

        switch (mode)
        {
        case PaintingMode::Brush:
            RenderBrushPreview(brush);
            break;
        case PaintingMode::Eraser:
            RenderEraserPreview();
            break;
        case PaintingMode::Fill:
            RenderFillPreview();
            break;
        default:
            RenderBasicHover();
            break;
        }
    }

    void PanelViewport::RenderBrushPreview(const Tile& brush)
    {
        glm::vec2 brushSize = brush.GetSize();

        Tiles::QuadParams params;
        params.Position = m_MouseFollowQuadPosition;
        params.Rotation = brush.GetRotation();
        params.Tint = brush.GetTint();
        params.Size = { m_TileSize * brushSize.x, m_TileSize * brushSize.y };

        const auto& textureAtlases = m_Context->GetProject()->GetTextureAtlases();
        if (brush.IsTextured() && brush.HasValidAtlas() && brush.GetAtlasIndex() < textureAtlases.size())
        {
            auto atlas = textureAtlases[brush.GetAtlasIndex()];
            if (atlas && atlas->HasTexture())
            {
                params.Texture = atlas->GetTexture();
                params.TexCoords = brush.GetTextureCoords();
            }
        }

        Tiles::Renderer2D::DrawQuad(params);
    }

    void PanelViewport::RenderEraserPreview()
    {
        Tiles::Renderer2D::DrawQuad({
            .Position = m_MouseFollowQuadPosition,
            .Size = { m_TileSize, m_TileSize },
            .Tint = { 1.0f, 0.0f, 0.0f, 0.3f },
        });
    }

    void PanelViewport::RenderFillPreview()
    {
        Tiles::Renderer2D::DrawQuad({
            .Position = m_MouseFollowQuadPosition,
            .Size = { m_TileSize, m_TileSize },
            .Tint = { 0.0f, 0.0f, 1.0f, 0.3f },
        });
    }

    void PanelViewport::RenderBasicHover()
    {
        Tiles::Renderer2D::DrawQuad({
            .Position = m_MouseFollowQuadPosition,
            .Size = m_MouseFollowQuadSize,
            .Tint = m_MouseFollowQuadColor,
        });
    }

    void PanelViewport::HandleInput()
    {
        // Don't paint through the overlay controls.
        if (m_PointerOverOverlay)
            return;

        glm::ivec2 gridPos = GetGridPositionUnderMouse();

        if (Input::IsMouseButtonPressed(MouseCode::Left))
        {
            ExecutePaintAction(gridPos);
        }
    }

    void PanelViewport::ExecutePaintAction(const glm::ivec2& gridPos)
    {
        PaintingMode mode = m_Context->GetPaintingMode();

        // Any signed coord is paintable; it maps directly to a sparse cell.
        switch (mode)
        {
        case PaintingMode::Brush:
            m_Context->PaintTile(gridPos.x, gridPos.y);
            break;
        case PaintingMode::Eraser:
            m_Context->EraseTile(gridPos.x, gridPos.y);
            break;
        case PaintingMode::Fill:
        {
            // Bound the flood to the visible tile region, so a fill on the
            // unbounded board fills what's on screen rather than running away.
            const Camera2D& camera = m_Context->GetViewportCamera();
            float halfWidth = m_ViewportSize.x / camera.Zoom * 0.5f;
            float halfHeight = m_ViewportSize.y / camera.Zoom * 0.5f;
            glm::ivec4 visibleTiles = {
                static_cast<int>(std::floor((camera.Center.x - halfWidth) / m_TileSize)),
                static_cast<int>(std::floor((camera.Center.y - halfHeight) / m_TileSize)),
                static_cast<int>(std::floor((camera.Center.x + halfWidth) / m_TileSize)),
                static_cast<int>(std::floor((camera.Center.y + halfHeight) / m_TileSize))
            };
            m_Context->FillLayer(gridPos.x, gridPos.y, visibleTiles);
            break;
        }
        default:
            break;
        }
    }

    glm::ivec2 PanelViewport::GetGridPositionUnderMouse() const
    {
        auto worldPosition = ScreenToWorld();
        // A tile at (cx, cy) fills the cell [cx, cx+1], so floor maps a world
        // point to the cell that contains it (correct for negatives too).
        return {
            static_cast<int>(std::floor(worldPosition.x / m_TileSize)),
            static_cast<int>(std::floor(worldPosition.y / m_TileSize))
        };
    }

    glm::vec2 PanelViewport::ScreenToWorld() const
    {
        const Camera2D& camera = m_Context->GetViewportCamera();
        glm::vec2 relativeMouse = {
            m_CurrentMousePosition.x - m_ViewportPosition.x,
            m_CurrentMousePosition.y - m_ViewportPosition.y
        };

        // The scene is rendered to a bottom-left-origin GL texture and shown via
        // ImGui::Image with default UVs, i.e. vertically mirrored. Camera2D only
        // inverts the projection, so undo that display mirror on the pixel Y here.
        relativeMouse.y = m_ViewportSize.y - relativeMouse.y;

        return camera.ScreenToWorld(relativeMouse, { m_ViewportSize.x, m_ViewportSize.y });
    }
}