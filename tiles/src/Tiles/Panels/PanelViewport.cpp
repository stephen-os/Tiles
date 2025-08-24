#include "PanelViewport.h"

#include "Core/Constants.h"

#include "Lumina/Core/Input.h"
#include <algorithm>

namespace Tiles
{
    PanelViewport::PanelViewport(Ref<Context> context)
        : Panel(context), m_TileSize(Viewport::Render::DefaultTileSize)
    {
        m_RenderTarget = Renderer2D::CreateRenderTarget(512, 512);
        m_MouseFollowQuadSize = { m_TileSize * 0.5f, m_TileSize * 0.5f };
        m_MouseFollowQuadColor = Viewport::Grid::HoverColor;
    }

    void PanelViewport::Render()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
        ImGui::Begin("Viewport", nullptr, flags);

        if (!m_Context || !m_Context->HasProject())
        {
            ImGui::TextColored(UI::Color::TextError, "No project loaded");
            ImGui::End();
            return;
        }

        auto camera = m_Context->GetViewportCamera();
        if (!camera)
        {
            ImGui::TextColored(UI::Color::TextError, "No camera available");
            ImGui::End();
            return;
        }

        m_MousePosition = ImGui::GetMousePos();
        m_ViewportPosition = ImGui::GetCursorScreenPos();
        m_ViewportSize = ImGui::GetContentRegionAvail();
        m_ViewportSize.x = std::max(m_ViewportSize.x, m_TileSize);
        m_ViewportSize.y = std::max(m_ViewportSize.y, m_TileSize);

        camera->SetSize(m_ViewportSize.x, m_ViewportSize.y);

        Renderer2D::SetRenderTarget(m_RenderTarget);
        Renderer2D::SetResolution(m_ViewportSize.x, m_ViewportSize.y);
        Renderer2D::Begin(camera);

        RenderGrid();
        RenderLayers();
        RenderHoverTile();
        HandleInput();

        Renderer2D::End();
        Renderer2D::SetRenderTarget(nullptr);

        Lumina::Renderer2D::ResetQuadState();
		Lumina::Renderer2D::ResetLineState();

        ImGui::Image((void*)m_RenderTarget->GetTexture(), m_ViewportSize);

        m_MouseDelta = ImGui::GetIO().MouseWheel;

        ImGui::End();
    }

    void PanelViewport::Update()
    {
        if (!m_Context || !m_Context->HasProject())
            return;

        HandleCameraMovement();
        HandleZoom();
    }

    void PanelViewport::HandleCameraMovement()
    {
        auto camera = m_Context->GetViewportCamera();
        if (!camera) return;

        if (Input::IsKeyPressed(KeyCode::W))
            camera->MoveUp(Viewport::Input::CameraMoveSpeed);
        if (Input::IsKeyPressed(KeyCode::S))
            camera->MoveUp(-Viewport::Input::CameraMoveSpeed);
        if (Input::IsKeyPressed(KeyCode::A))
            camera->MoveRight(Viewport::Input::CameraMoveSpeed);
        if (Input::IsKeyPressed(KeyCode::D))
            camera->MoveRight(-Viewport::Input::CameraMoveSpeed);
    }

    void PanelViewport::HandleZoom()
    {
        auto camera = m_Context->GetViewportCamera();
        if (!camera || m_MouseDelta == 0) return;

        float zoomChange = m_MouseDelta * Viewport::Render::ZoomSensitivity;
        float newZoom = std::clamp(
            camera->GetZoom() + zoomChange,
            Viewport::Render::MinZoom,
            Viewport::Render::MaxZoom
        );
        camera->SetZoom(newZoom);
    }

    void PanelViewport::RenderGrid()
    {
        auto camera = m_Context->GetViewportCamera();
        glm::vec3 cameraPos = camera->GetPosition();
        const LayerStack& layerStack = m_Context->GetProject()->GetLayerStack();

        Renderer2D::SetGridPosition({
            cameraPos.x + m_TileSize / 2,
            cameraPos.y + m_TileSize / 2,
            Viewport::Depth::Grid
            });
        Renderer2D::SetGridSize({ 16384.0f, 16384.0f });
        Renderer2D::SetGridCellSize(m_TileSize / 2);
        Renderer2D::SetGridColor(Viewport::Grid::GridColor);
        Renderer2D::SetGridLineWidth(2.0f);
        Renderer2D::SetGridShowCheckerboard(true);
        Renderer2D::SetGridCheckerColor1(Viewport::Grid::CheckerColor1);
        Renderer2D::SetGridCheckerColor2(Viewport::Grid::CheckerColor2);
        Renderer2D::DrawGrid();

        RenderLayerBoundaries();
    }

    void PanelViewport::RenderLayerBoundaries()
    {
        auto camera = m_Context->GetViewportCamera();
        glm::vec3 cameraPos = camera->GetPosition();
        const LayerStack& layerStack = m_Context->GetProject()->GetLayerStack();

        const float gridWidth = layerStack.GetWidth();
        const float gridHeight = layerStack.GetHeight();
        const float offset = m_TileSize * 0.5f;

        Renderer2D::SetLineColor(Viewport::Grid::BoundaryColor);
        Renderer2D::SetLineThickness(2.0f);

        Renderer2D::SetLineStart({
            offset + cameraPos.x,
            offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Renderer2D::SetLineEnd({
            m_TileSize * gridWidth + offset + cameraPos.x,
            offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Renderer2D::DrawLine();

        Renderer2D::SetLineStart({
            offset + cameraPos.x,
            offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Renderer2D::SetLineEnd({
            offset + cameraPos.x,
            m_TileSize * gridHeight + offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Renderer2D::DrawLine();

        Renderer2D::SetLineStart({
            m_TileSize * gridWidth + offset + cameraPos.x,
            m_TileSize * gridHeight + offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Renderer2D::SetLineEnd({
            offset + cameraPos.x,
            m_TileSize * gridHeight + offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Renderer2D::DrawLine();

        Renderer2D::SetLineStart({
            m_TileSize * gridWidth + offset + cameraPos.x,
            m_TileSize * gridHeight + offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Renderer2D::SetLineEnd({
            m_TileSize * gridWidth + offset + cameraPos.x,
            offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Renderer2D::DrawLine();
    }

    void PanelViewport::RenderLayers()
    {
        const LayerStack& layerStack = m_Context->GetProject()->GetLayerStack();
        auto camera = m_Context->GetViewportCamera();
        glm::vec3 cameraPos = camera->GetPosition();

        for (size_t layerIdx = 0; layerIdx < layerStack.GetLayerCount(); ++layerIdx)
        {
            const TileLayer& layer = layerStack.GetLayer(layerIdx);
            if (!layer.GetVisibility()) continue;
            RenderLayer(layer, layerIdx, cameraPos);
        }
    }

    void PanelViewport::RenderLayer(const TileLayer& layer, size_t layerIndex, const glm::vec3& cameraPos)
    {
        const auto& textureAtlases = m_Context->GetProject()->GetTextureAtlases();

        for (size_t y = 0; y < layer.GetHeight(); ++y)
        {
            for (size_t x = 0; x < layer.GetWidth(); ++x)
            {
                const Tile& tile = layer.GetTile(x, y);
                if (!tile.IsPainted()) continue;

                glm::vec2 tileWorldPos = {
                    (x + 1) * m_TileSize + cameraPos.x,
                    (y + 1) * m_TileSize + cameraPos.y
                };

                Renderer2D::SetQuadPosition({
                    tileWorldPos.x,
                    tileWorldPos.y,
                    Viewport::Depth::Tile + layerIndex * 0.01f
                    });
                Renderer2D::SetQuadRotation(tile.GetRotation());
                Renderer2D::SetQuadTintColor(tile.GetTint());

                glm::vec2 tileSize = tile.GetSize();
                Renderer2D::SetQuadSize({
                    m_TileSize * tileSize.x,
                    m_TileSize * tileSize.y
                    });

                if (tile.IsTextured() && tile.GetAtlasIndex() < textureAtlases.size())
                {
                    auto atlas = textureAtlases[tile.GetAtlasIndex()];
                    if (atlas && atlas->HasTexture())
                    {
                        Renderer2D::SetQuadTexture(atlas->GetTexture());
                        Renderer2D::SetQuadTextureCoords(tile.GetTextureCoords());
                    }
                    else
                    {
                        Renderer2D::SetQuadTexture(nullptr);
                        Renderer2D::SetQuadTextureCoords({ 0.0f, 0.0f, 1.0f, 1.0f });
                    }
                }
                else
                {
                    Renderer2D::SetQuadTexture(nullptr);
                    Renderer2D::SetQuadTextureCoords({ 0.0f, 0.0f, 1.0f, 1.0f });
                }

                Renderer2D::DrawQuad();
            }
        }
    }

    void PanelViewport::RenderHoverTile()
    {
        if (!ImGui::IsWindowHovered()) return;

        const LayerStack& layerStack = m_Context->GetProject()->GetLayerStack();
        auto camera = m_Context->GetViewportCamera();
        glm::vec3 cameraPos = camera->GetPosition();

        glm::ivec2 gridPos = GetGridPositionUnderMouse();
        if (!IsValidGridPosition(gridPos)) return;

        glm::vec2 gridCenter = {
            gridPos.x * m_TileSize + cameraPos.x,
            gridPos.y * m_TileSize + cameraPos.y
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
            RenderBrushPreview(brush, cameraPos);
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

    void PanelViewport::RenderBrushPreview(const Tile& brush, const glm::vec3& cameraPos)
    {
        Renderer2D::SetQuadPosition(m_MouseFollowQuadPosition);
        Renderer2D::SetQuadRotation(brush.GetRotation());
        Renderer2D::SetQuadTintColor(brush.GetTint());

        glm::vec2 brushSize = brush.GetSize();
        Renderer2D::SetQuadSize({
            m_TileSize * brushSize.x,
            m_TileSize * brushSize.y
            });

        const auto& textureAtlases = m_Context->GetProject()->GetTextureAtlases();
        if (brush.IsTextured() && brush.HasValidAtlas() && brush.GetAtlasIndex() < textureAtlases.size())
        {
            auto atlas = textureAtlases[brush.GetAtlasIndex()];
            if (atlas && atlas->HasTexture())
            {
                Renderer2D::SetQuadTexture(atlas->GetTexture());
                Renderer2D::SetQuadTextureCoords(brush.GetTextureCoords());
            }
            else
            {
                Renderer2D::SetQuadTexture(nullptr);
            }
        }
        else
        {
            Renderer2D::SetQuadTexture(nullptr);
        }

        Renderer2D::DrawQuad();
    }

    void PanelViewport::RenderEraserPreview()
    {
        Renderer2D::SetQuadPosition(m_MouseFollowQuadPosition);
        Renderer2D::SetQuadSize({ m_TileSize, m_TileSize });
        Renderer2D::SetQuadRotation({ 0.0f, 0.0f, 0.0f });
        Renderer2D::SetQuadTintColor({ 1.0f, 0.0f, 0.0f, 0.3f });
        Renderer2D::SetQuadTexture(nullptr);
        Renderer2D::DrawQuad();
    }

    void PanelViewport::RenderFillPreview()
    {
        Renderer2D::SetQuadPosition(m_MouseFollowQuadPosition);
        Renderer2D::SetQuadSize({ m_TileSize, m_TileSize });
        Renderer2D::SetQuadRotation({ 0.0f, 0.0f, 0.0f });
        Renderer2D::SetQuadTintColor({ 0.0f, 0.0f, 1.0f, 0.3f });
        Renderer2D::SetQuadTexture(nullptr);
        Renderer2D::DrawQuad();
    }

    void PanelViewport::RenderBasicHover()
    {
        Renderer2D::SetQuadPosition(m_MouseFollowQuadPosition);
        Renderer2D::SetQuadSize(m_MouseFollowQuadSize);
        Renderer2D::SetQuadRotation({ 0.0f, 0.0f, 0.0f });
        Renderer2D::SetQuadTintColor(m_MouseFollowQuadColor);
        Renderer2D::SetQuadTexture(nullptr);
        Renderer2D::DrawQuad();
    }

    void PanelViewport::HandleInput()
    {
        if (!ImGui::IsWindowHovered()) return;

        glm::ivec2 gridPos = GetGridPositionUnderMouse();
        if (!IsValidGridPosition(gridPos)) return;

        if (Input::IsMouseButtonPressed(MouseCode::Left))
        {
            ExecutePaintAction(gridPos);
        }
    }

    void PanelViewport::ExecutePaintAction(const glm::ivec2& gridPos)
    {
        PaintingMode mode = m_Context->GetPaintingMode();

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
        auto camera = m_Context->GetViewportCamera();
        glm::vec3 cameraPosition = camera->GetPosition();
        float zoom = camera->GetZoom();

        ImVec2 relativeMousePosition = {
            m_MousePosition.x - m_ViewportPosition.x,
            m_MousePosition.y - m_ViewportPosition.y
        };

        float centeredX = relativeMousePosition.x - m_ViewportSize.x / 2.0f;
        float centeredY = relativeMousePosition.y - m_ViewportSize.y / 2.0f;

        float worldX = cameraPosition.x + centeredX / zoom;
        float worldY = cameraPosition.y - centeredY / zoom;

        return { worldX, worldY };
    }
}