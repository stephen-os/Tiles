#include "PanelViewport.h"

#include "Core/Constants.h"
#include "../UIConstants.h"

#include "Core/Input.h"
#include "../Rendering/TileSceneRenderer.h"
#include <algorithm>

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
        RenderHoverTile();

        Tiles::Renderer2D::EndFrame();
        Tiles::Renderer2D::SetRenderTarget(nullptr);


        ImGui::Image((void*)m_RenderTarget->GetTexture(), m_ViewportSize);

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
        // space from the frame's view-projection (set by BeginFrame). One grid
        // cell per tile.
        Tiles::Renderer2D::DrawGrid({ .CellSize = m_TileSize });

        RenderLayerBoundaries();
    }

    void PanelViewport::RenderLayerBoundaries()
    {
        const LayerStack& layerStack = m_Context->GetProject()->GetLayerStack();

        const float gridWidth = layerStack.GetWidth();
        const float gridHeight = layerStack.GetHeight();
        const float offset = m_TileSize * 0.5f;

        Tiles::LineParams line;
        line.Color = Viewport::Grid::BoundaryColor;
        line.Thickness = 2.0f;

        line.Start = {
            offset,
            offset,
            Viewport::Depth::Outline
            };
        line.End = {
            m_TileSize * gridWidth + offset,
            offset,
            Viewport::Depth::Outline
            };
        Tiles::Renderer2D::DrawLine(line);

        line.Start = {
            offset,
            offset,
            Viewport::Depth::Outline
            };
        line.End = {
            offset,
            m_TileSize * gridHeight + offset,
            Viewport::Depth::Outline
            };
        Tiles::Renderer2D::DrawLine(line);

        line.Start = {
            m_TileSize * gridWidth + offset,
            m_TileSize * gridHeight + offset,
            Viewport::Depth::Outline
            };
        line.End = {
            offset,
            m_TileSize * gridHeight + offset,
            Viewport::Depth::Outline
            };
        Tiles::Renderer2D::DrawLine(line);

        line.Start = {
            m_TileSize * gridWidth + offset,
            m_TileSize * gridHeight + offset,
            Viewport::Depth::Outline
            };
        line.End = {
            m_TileSize * gridWidth + offset,
            offset,
            Viewport::Depth::Outline
            };
        Tiles::Renderer2D::DrawLine(line);
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

        const LayerStack& layerStack = m_Context->GetProject()->GetLayerStack();

        glm::ivec2 gridPos = GetGridPositionUnderMouse();
        if (!IsValidGridPosition(gridPos)) return;

        glm::vec2 gridCenter = {
            gridPos.x * m_TileSize,
            gridPos.y * m_TileSize
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
        glm::ivec2 gridPos = GetGridPositionUnderMouse();
        if (!IsValidGridPosition(gridPos)) 
            return;

        if (Input::IsMouseButtonPressed(MouseCode::Left))
        {
            ExecutePaintAction(gridPos);
        }
    }

    void PanelViewport::ExecutePaintAction(const glm::ivec2& gridPos)
    {
        PaintingMode mode = m_Context->GetPaintingMode();

        // gridPos is 1-based (see GetGridPositionUnderMouse); layers are indexed
        // from zero, hence the -1 on each coordinate.
        switch (mode)
        {
        case PaintingMode::Brush:
            m_Context->PaintTile(gridPos.x - 1, gridPos.y - 1);
            break;
        case PaintingMode::Eraser:
            m_Context->EraseTile(gridPos.x - 1, gridPos.y - 1);
            break;
        case PaintingMode::Fill:
            m_Context->FillLayer(gridPos.x - 1, gridPos.y - 1);
            break;
        default:
            break;
        }
    }

    glm::ivec2 PanelViewport::GetGridPositionUnderMouse() const
    {
        auto worldPosition = ScreenToWorld();
        return {
            static_cast<int>(round(worldPosition.x / m_TileSize)),
            static_cast<int>(round(worldPosition.y / m_TileSize))
        };
    }

    bool PanelViewport::IsValidGridPosition(const glm::ivec2& gridPos) const
    {
        const LayerStack& layerStack = m_Context->GetProject()->GetLayerStack();
        return gridPos.x >= 1 && gridPos.x <= static_cast<int>(layerStack.GetWidth()) &&
            gridPos.y >= 1 && gridPos.y <= static_cast<int>(layerStack.GetHeight());
    }

    glm::vec2 PanelViewport::ScreenToWorld() const
    {
        const Camera2D& camera = m_Context->GetViewportCamera();
        glm::vec2 relativeMouse = {
            m_CurrentMousePosition.x - m_ViewportPosition.x,
            m_CurrentMousePosition.y - m_ViewportPosition.y
        };
        return camera.ScreenToWorld(relativeMouse, { m_ViewportSize.x, m_ViewportSize.y });
    }
}