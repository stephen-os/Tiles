#include "PanelViewport.h"

#include "../UIConstants.h"
#include "../UI/Theme.h"

#include "Core/Input.h"
#include "../Rendering/TileSceneRenderer.h"
#include <algorithm>
#include <cmath>

namespace Tiles::Editor
{
    PanelViewport::PanelViewport(EditorHost& host)
        : Panel(host), m_TileSize(Viewport::Render::DefaultTileSize)
    {
        m_RenderTarget = Tiles::RenderTarget::Create(512, 512);
    }

    void PanelViewport::Render()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
        ImGui::Begin("Viewport", OpenFlag(), flags);

		m_IsWindowFocused = ImGui::IsWindowFocused();

        if (!Ctx().HasProject())
        {
            ImGui::TextColored(UI::GetTheme().Danger, "No project loaded");
            ImGui::End();
            return;
        }

        Camera2D& camera = Ctx().GetViewportCamera();

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

        ImGui::Image(static_cast<ImTextureID>(m_RenderTarget->GetTexture()), m_ViewportSize);

        RenderOverlay();

        m_MouseDelta = ImGui::GetIO().MouseWheel;

        ImGui::End();
    }

    void PanelViewport::Update()
    {
        if (!Ctx().HasProject())
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

        Camera2D& camera = Ctx().GetViewportCamera();

        if (Input::IsKeyDown(Input::KeyCode::W))
            camera.Center.y += Viewport::Input::CameraMoveSpeed;
        if (Input::IsKeyDown(Input::KeyCode::S))
            camera.Center.y -= Viewport::Input::CameraMoveSpeed;
        if (Input::IsKeyDown(Input::KeyCode::A))
            camera.Center.x -= Viewport::Input::CameraMoveSpeed;
        if (Input::IsKeyDown(Input::KeyCode::D))
            camera.Center.x += Viewport::Input::CameraMoveSpeed;

        if (Input::IsKeyDown(Input::KeyCode::Q))
            camera.Zoom = std::min(camera.Zoom * 1.02f, Viewport::Render::MaxZoom);
        if (Input::IsKeyDown(Input::KeyCode::E))
            camera.Zoom = std::max(camera.Zoom / 1.02f, Viewport::Render::MinZoom);
    }

    void PanelViewport::HandleZoom()
    {
        Camera2D& camera = Ctx().GetViewportCamera();
        if (m_MouseDelta == 0) return;

        camera.Zoom = std::clamp(
            camera.Zoom + m_MouseDelta * Viewport::Render::ZoomSensitivity,
            Viewport::Render::MinZoom,
            Viewport::Render::MaxZoom
        );
    }

    void PanelViewport::HandleMouseDragging()
    {
        Camera2D& camera = Ctx().GetViewportCamera();

        // A pan begins on middle-button press and ends only when middle is
        // released; while dragging, either middle or right may hold the pan.
        // World Y grows upward, so the vertical delta is added rather than subtracted.
        if (Input::IsMouseButtonDown(Input::MouseCode::Middle) && !m_IsDragging)
        {
            m_IsDragging = true;
            m_PreviousMousePosition = m_CurrentMousePosition;
        }

        if (m_IsDragging && (Input::IsMouseButtonDown(Input::MouseCode::Right) || Input::IsMouseButtonDown(Input::MouseCode::Middle)))
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

        if (m_IsDragging && !Input::IsMouseButtonDown(Input::MouseCode::Middle))
        {
            m_IsDragging = false;
        }
    }

    void PanelViewport::RenderGrid()
    {
        // Colors come from the Grid struct's defaults; the grid reconstructs world
        // space from the frame's view-projection (set by BeginFrame). Grid lines
        // fall on integer tile boundaries (so they pass through the origin); a
        // tile at (cx, cy) fills the cell between them.
        Tiles::Renderer2D::DrawGrid({ .CellSize = m_TileSize });
    }

    void PanelViewport::RenderOrigin()
    {
        const Camera2D& camera = Ctx().GetViewportCamera();

        // Axes span the visible extent so they read as reference lines through the
        // world origin (only visible when the origin is in view).
        float halfWidth = m_ViewportSize.x / camera.Zoom * 0.5f;
        float halfHeight = m_ViewportSize.y / camera.Zoom * 0.5f;

        Tiles::Line line;
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
        const ExportRegion& region = Ctx().GetProject()->GetExportRegion();
        if (!region.Enabled)
            return;

        // Region covers tiles [Min, Min+Size); a tile at (cx,cy) fills cell
        // [cx, cx+1], so the rectangle spans Min*tileSize to (Min+Size)*tileSize.
        float x0 = region.Min.x * m_TileSize;
        float y0 = region.Min.y * m_TileSize;
        float x1 = (region.Min.x + region.Size.x) * m_TileSize;
        float y1 = (region.Min.y + region.Size.y) * m_TileSize;

        Tiles::Line line;
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
        Camera2D& camera = Ctx().GetViewportCamera();

        ImGui::SetCursorScreenPos({ m_ViewportPosition.x + 8.0f, m_ViewportPosition.y + 8.0f });
        ImGui::BeginGroup();

        if (ImGui::Button("Origin"))
            camera.Center = { 0.0f, 0.0f };
        ImGui::SameLine();
        if (ImGui::Button("Fit"))
            camera.Fit(Ctx().GetProject()->GetLayerStack().GetBounds(),
                Viewport::Render::DefaultTileSize, Viewport::Render::MinZoom, Viewport::Render::MaxZoom);

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
        const LayerStack& layerStack = Ctx().GetProject()->GetLayerStack();

        for (size_t layerIdx = 0; layerIdx < layerStack.GetLayerCount(); ++layerIdx)
        {
            const TileLayer& layer = layerStack.GetLayer(layerIdx);
            if (!layer.GetVisibility()) continue;
            RenderLayer(layer, layerIdx);
        }
    }

    void PanelViewport::RenderLayer(const TileLayer& layer, size_t layerIndex)
    {
        const auto& textureAtlases = Ctx().GetProject()->GetTextureAtlases();
        DrawTileLayer(layer, layerIndex, m_TileSize, textureAtlases, Viewport::Depth::Tile, Host());
    }

    void PanelViewport::RenderHoverTile()
    {
        PaintingMode mode = Ctx().GetPaintingMode();
        if (mode == PaintingMode::None)
            return;

        // While a stroke is in progress, preview every accumulated cell.
        if (m_Stroking)
        {
            for (const glm::ivec2& cell : m_StrokeCells)
                RenderCellPreview(cell);
            return;
        }

        if (!ImGui::IsWindowHovered())
            return;

        glm::ivec2 cursor = GetGridPositionUnderMouse();

        // Fill and shapes (before the drag) preview a single cell; brush/eraser
        // preview the whole footprint.
        if (mode == PaintingMode::Fill || IsShapeMode(mode))
        {
            RenderCellPreview(cursor);
            return;
        }

        for (const glm::ivec2& cell : Ctx().GetBrushFootprint(cursor.x, cursor.y))
            RenderCellPreview(cell);
    }

    void PanelViewport::RenderCellPreview(const glm::ivec2& cell)
    {
        glm::vec3 position = {
            (cell.x + 0.5f) * m_TileSize,
            (cell.y + 0.5f) * m_TileSize,
            Viewport::Depth::HoverTile
        };

        Tiles::Square params;
        params.Position = position;
        params.Size = { m_TileSize, m_TileSize };

        switch (Ctx().GetPaintingMode())
        {
        case PaintingMode::Line:
        case PaintingMode::Rectangle:
        case PaintingMode::Ellipse:
        case PaintingMode::Brush:
        {
            const Tile& brush = Ctx().GetBrush();
            params.Rotation = brush.GetRotation();
            params.Tint = brush.GetTint();
            params.Size = { m_TileSize * brush.GetSize().x, m_TileSize * brush.GetSize().y };

            if (brush.IsTextured() && brush.HasValidAtlas())
            {
                auto atlas = Ctx().GetProject()->GetTextureAtlasById(brush.GetAtlasId());
                if (atlas && atlas->HasImage())
                {
                    params.Texture = Host().GetAtlasTexture(*atlas);
                    params.TexCoords = atlas->GetTextureCoords(brush.GetCellIndex());
                }
            }
            break;
        }
        case PaintingMode::Eraser:
            params.Tint = { 1.0f, 0.0f, 0.0f, 0.3f };
            break;
        case PaintingMode::Fill:
            params.Tint = { 0.0f, 0.0f, 1.0f, 0.3f };
            break;
        default:
            return;
        }

        Tiles::Renderer2D::DrawSquare(params);
    }

    void PanelViewport::HandleInput()
    {
        PaintingMode mode = Ctx().GetPaintingMode();

        // Always finish an in-progress stroke on release, even if the pointer
        // ended over the overlay controls.
        if (m_Stroking && Input::IsMouseButtonReleased(Input::MouseCode::Left))
        {
            CommitStroke();
            return;
        }

        // Don't start painting through the overlay controls.
        if (m_PointerOverOverlay)
            return;

        glm::ivec2 gridPos = GetGridPositionUnderMouse();

        if (mode == PaintingMode::Fill)
        {
            if (Input::IsMouseButtonPressed(Input::MouseCode::Left))
                FillAt(gridPos);
            return;
        }

        if (IsShapeMode(mode))
        {
            if (Input::IsMouseButtonPressed(Input::MouseCode::Left))
                BeginShape(gridPos);
            else if (m_Stroking && Input::IsMouseButtonDown(Input::MouseCode::Left))
                UpdateShape(gridPos);
            return;
        }

        if (mode != PaintingMode::Brush && mode != PaintingMode::Eraser)
            return;

        if (Input::IsMouseButtonPressed(Input::MouseCode::Left))
            BeginStroke(gridPos);
        else if (m_Stroking && Input::IsMouseButtonDown(Input::MouseCode::Left))
            ExtendStroke(gridPos);
    }

    void PanelViewport::BeginStroke(const glm::ivec2& cell)
    {
        m_Stroking = true;
        m_StrokeCells.clear();
        m_LastStrokeCell = cell;
        AddFootprint(cell);
    }

    void PanelViewport::ExtendStroke(const glm::ivec2& cell)
    {
        // Only extend when the cursor enters a new cell, so a held mouse doesn't
        // pile up the same footprint every frame.
        if (cell == m_LastStrokeCell)
            return;

        m_LastStrokeCell = cell;
        AddFootprint(cell);
    }

    void PanelViewport::AddFootprint(const glm::ivec2& cell)
    {
        for (const glm::ivec2& footprintCell : Ctx().GetBrushFootprint(cell.x, cell.y))
            m_StrokeCells.push_back(footprintCell);
    }

    void PanelViewport::CommitStroke()
    {
        if (!m_Stroking)
            return;

        Ctx().PaintStroke(m_StrokeCells);
        m_Stroking = false;
        m_StrokeCells.clear();
    }

    void PanelViewport::BeginShape(const glm::ivec2& cell)
    {
        m_Stroking = true;
        m_ShapeAnchor = cell;
        m_StrokeCells = Ctx().GetShapeCells(cell, cell);
    }

    void PanelViewport::UpdateShape(const glm::ivec2& cell)
    {
        // Recompute the whole shape from the anchor each frame (unlike a freehand
        // stroke, which accumulates cells).
        m_StrokeCells = Ctx().GetShapeCells(m_ShapeAnchor, cell);
    }

    void PanelViewport::FillAt(const glm::ivec2& cell)
    {
        // Bound the flood to the visible tile region, so a fill on the unbounded
        // board fills what's on screen rather than running away.
        const Camera2D& camera = Ctx().GetViewportCamera();
        float halfWidth = m_ViewportSize.x / camera.Zoom * 0.5f;
        float halfHeight = m_ViewportSize.y / camera.Zoom * 0.5f;
        glm::ivec4 visibleTiles = {
            static_cast<int>(std::floor((camera.Center.x - halfWidth) / m_TileSize)),
            static_cast<int>(std::floor((camera.Center.y - halfHeight) / m_TileSize)),
            static_cast<int>(std::floor((camera.Center.x + halfWidth) / m_TileSize)),
            static_cast<int>(std::floor((camera.Center.y + halfHeight) / m_TileSize))
        };
        Ctx().FillLayer(cell.x, cell.y, visibleTiles);
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
        const Camera2D& camera = Ctx().GetViewportCamera();
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