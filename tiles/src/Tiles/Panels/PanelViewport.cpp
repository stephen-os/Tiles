#include "PanelViewport.h"
#include "Constants.h"
#include "Lumina/Core/Log.h"
#include "Lumina/Core/Input.h"
#include <algorithm>

namespace Tiles
{
    PanelViewport::PanelViewport(Ref<Context> context) : Panel(context), m_TileSize(Viewport::Render::DefaultTileSize)
    {
        m_RenderTarget = Lumina::Renderer2D::CreateRenderTarget(512, 512);
        m_Camera = Lumina::CreateRef<Lumina::OrthographicCamera>();

        // Initialize camera position to center of a default 16x16 grid
        const float gridWidth = 16.0f;
        const float gridHeight = 16.0f;

        m_Camera->SetPosition({
            m_TileSize * (gridWidth * 0.5f) + m_TileSize * 0.5f,
            m_TileSize * (gridHeight * 0.5f) + m_TileSize * 0.5f,
            1.0f
            });
        m_Camera->SetZoom(1.0f);

        // Initialize hover tile appearance
        m_MouseFollowQuadSize = { m_TileSize * 0.5f, m_TileSize * 0.5f };
        m_MouseFollowQuadColor = Viewport::Grid::HoverColor;
    }

    void PanelViewport::Render()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
        ImGui::Begin("Viewport", nullptr, flags);

        if (!m_Context)
        {
            ImGui::TextColored(UI::Color::TextError, "No project loaded");
            ImGui::End();
            return;
        }

        // Update viewport state
        m_MousePosition = ImGui::GetMousePos();
        m_ViewportPosition = ImGui::GetCursorScreenPos();
        m_ViewportSize = ImGui::GetContentRegionAvail();
        m_ViewportSize.x = std::max(m_ViewportSize.x, m_TileSize);
        m_ViewportSize.y = std::max(m_ViewportSize.y, m_TileSize);

        m_Camera->SetSize(m_ViewportSize.x, m_ViewportSize.y);

        // Begin 3D rendering
        Lumina::Renderer2D::SetRenderTarget(m_RenderTarget);
        Lumina::Renderer2D::SetResolution(m_ViewportSize.x, m_ViewportSize.y);
        Lumina::Renderer2D::Begin(m_Camera);

        RenderGrid();
        RenderLayers();
        RenderHoverTile();
        HandleInput();

        Lumina::Renderer2D::End();
        Lumina::Renderer2D::SetRenderTarget(nullptr);

        // Display the rendered viewport
        ImGui::Image((void*)m_RenderTarget->GetTexture(), m_ViewportSize);

        // Update mouse wheel delta for zoom
        m_MouseDelta = ImGui::GetIO().MouseWheel;

        ImGui::End();
    }

    void PanelViewport::Update()
    {
        if (!m_Context) return;

        // Camera movement with WASD
        if (Lumina::Input::IsKeyPressed(Lumina::KeyCode::W))
            m_Camera->MoveUp(Viewport::Input::CameraMoveSpeed);
        if (Lumina::Input::IsKeyPressed(Lumina::KeyCode::S))
            m_Camera->MoveUp(-Viewport::Input::CameraMoveSpeed);
        if (Lumina::Input::IsKeyPressed(Lumina::KeyCode::A))
            m_Camera->MoveRight(Viewport::Input::CameraMoveSpeed);
        if (Lumina::Input::IsKeyPressed(Lumina::KeyCode::D))
            m_Camera->MoveRight(-Viewport::Input::CameraMoveSpeed);

        // Handle zoom with mouse wheel
        if (m_MouseDelta != 0)
        {
            float zoomChange = m_MouseDelta * Viewport::Render::ZoomSensitivity;
            float newZoom = std::clamp(
                m_Camera->GetZoom() + zoomChange,
                Viewport::Render::MinZoom,
                Viewport::Render::MaxZoom
            );
            m_Camera->SetZoom(newZoom);
        }
    }

    void PanelViewport::RenderGrid()
    {
        glm::vec3 cameraPos = m_Camera->GetPosition();
        const LayerStack& layerStack = m_Context->GetProject()->GetLayerStack();

        // Render infinite checkerboard pattern
        Lumina::Renderer2D::SetGridPosition({
            cameraPos.x + m_TileSize / 2,
            cameraPos.y + m_TileSize / 2,
            Viewport::Depth::Grid
            });
        Lumina::Renderer2D::SetGridSize({ 16384.0f, 16384.0f });
        Lumina::Renderer2D::SetGridCellSize(m_TileSize / 2);
        Lumina::Renderer2D::SetGridColor(Viewport::Grid::GridColor);
        Lumina::Renderer2D::SetGridLineWidth(2.0f);
        Lumina::Renderer2D::SetGridShowCheckerboard(true);
        Lumina::Renderer2D::SetGridCheckerColor1(Viewport::Grid::CheckerColor1);
        Lumina::Renderer2D::SetGridCheckerColor2(Viewport::Grid::CheckerColor2);
        Lumina::Renderer2D::DrawGrid();

        // Render layer boundaries
        RenderLayerBoundaries();
    }

    void PanelViewport::RenderLayerBoundaries()
    {
        glm::vec3 cameraPos = m_Camera->GetPosition();
        const LayerStack& layerStack = m_Context->GetProject()->GetLayerStack();

        const float gridWidth = layerStack.GetWidth();
        const float gridHeight = layerStack.GetHeight();
        const float offset = m_TileSize * 0.5f;

        Lumina::Renderer2D::SetLineColor(Viewport::Grid::BoundaryColor);
        Lumina::Renderer2D::SetLineThickness(2.0f);

        // Top boundary
        Lumina::Renderer2D::SetLineStart({
            offset + cameraPos.x,
            offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Lumina::Renderer2D::SetLineEnd({
            m_TileSize * gridWidth + offset + cameraPos.x,
            offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Lumina::Renderer2D::DrawLine();

        // Left boundary
        Lumina::Renderer2D::SetLineStart({
            offset + cameraPos.x,
            offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Lumina::Renderer2D::SetLineEnd({
            offset + cameraPos.x,
            m_TileSize * gridHeight + offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Lumina::Renderer2D::DrawLine();

        // Bottom boundary
        Lumina::Renderer2D::SetLineStart({
            m_TileSize * gridWidth + offset + cameraPos.x,
            m_TileSize * gridHeight + offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Lumina::Renderer2D::SetLineEnd({
            offset + cameraPos.x,
            m_TileSize * gridHeight + offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Lumina::Renderer2D::DrawLine();

        // Right boundary
        Lumina::Renderer2D::SetLineStart({
            m_TileSize * gridWidth + offset + cameraPos.x,
            m_TileSize * gridHeight + offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Lumina::Renderer2D::SetLineEnd({
            m_TileSize * gridWidth + offset + cameraPos.x,
            offset + cameraPos.y,
            Viewport::Depth::Outline
            });
        Lumina::Renderer2D::DrawLine();
    }

    void PanelViewport::RenderLayers()
    {
        const LayerStack& layerStack = m_Context->GetProject()->GetLayerStack();
        glm::vec3 cameraPos = m_Camera->GetPosition();

        // Render layers from bottom to top (reverse iteration for proper layering)
        for (size_t layerIdx = 0; layerIdx < layerStack.GetLayerCount(); ++layerIdx)
        {
            const TileLayer& layer = layerStack.GetLayer(layerIdx);

            // Skip invisible layers
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

				// Skip unpainted tiles
                if (!tile.IsPainted()) continue;

                // Calculate world position for this tile
                glm::vec2 tileWorldPos = {
                    (x + 1) * m_TileSize + cameraPos.x,
                    (y + 1) * m_TileSize + cameraPos.y
                };

                // Set tile properties
                Lumina::Renderer2D::SetQuadPosition({
                    tileWorldPos.x,
                    tileWorldPos.y,
                    Viewport::Depth::Tile + layerIndex * 0.01f // Slight depth offset per layer
                    });
                Lumina::Renderer2D::SetQuadRotation(tile.GetRotation());
                Lumina::Renderer2D::SetQuadTintColor(tile.GetTint());

                glm::vec2 tileSize = tile.GetSize();
                Lumina::Renderer2D::SetQuadSize({
                    m_TileSize * tileSize.x,
                    m_TileSize * tileSize.y
                    });

                // Set texture if available
                if (tile.IsTextured())
                {
                    if (tile.GetAtlasIndex() < textureAtlases.size())
                    {
                        auto atlas = textureAtlases[tile.GetAtlasIndex()];
                        if (atlas && atlas->HasTexture())
                        {

                            Lumina::Renderer2D::SetQuadTexture(atlas->GetTexture());
                            Lumina::Renderer2D::SetQuadTextureCoords(tile.GetTextureCoords());
                        }
                    }
                }
                else
                {
                    Lumina::Renderer2D::SetQuadTexture(nullptr);
                    Lumina::Renderer2D::SetQuadTextureCoords({ 0.0f, 0.0f, 1.0f, 1.0f });
                }

                Lumina::Renderer2D::DrawQuad();
            }
        }
    }

    void PanelViewport::RenderHoverTile()
    {
        if (!ImGui::IsWindowHovered()) return;

        const LayerStack& layerStack = m_Context->GetProject()->GetLayerStack();
        glm::vec3 cameraPos = m_Camera->GetPosition();

        // Get grid position under mouse
        glm::ivec2 gridPos = GetGridPositionUnderMouse();

        // Check if mouse is within valid grid bounds
        if (!IsValidGridPosition(gridPos)) return;

        // Calculate world position for hover tile
        glm::vec2 gridCenter = {
            gridPos.x * m_TileSize + cameraPos.x,
            gridPos.y * m_TileSize + cameraPos.y
        };

        // Update mouse follow quad position
        m_MouseFollowQuadPosition = {
            gridCenter.x,
            gridCenter.y,
            Viewport::Depth::HoverTile
        };

        // Get current brush for preview
        const Tile& brush = m_Context->GetBrush();

        // Render hover tile based on current painting mode
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
            // Show basic hover outline for no tool selected
            RenderBasicHover();
            break;
        }
    }

    void PanelViewport::RenderBrushPreview(const Tile& brush, const glm::vec3& cameraPos)
    {
        Lumina::Renderer2D::SetQuadPosition(m_MouseFollowQuadPosition);
        Lumina::Renderer2D::SetQuadRotation(brush.GetRotation());
        Lumina::Renderer2D::SetQuadTintColor(brush.GetTint());

        glm::vec2 brushSize = brush.GetSize();
        Lumina::Renderer2D::SetQuadSize({
            m_TileSize * brushSize.x,
            m_TileSize * brushSize.y
            });

        // Set texture if brush has one
        const auto& textureAtlases = m_Context->GetProject()->GetTextureAtlases();
        if (brush.IsTextured() && brush.HasValidAtlas() &&
            brush.GetAtlasIndex() < textureAtlases.size())
        {
            auto atlas = textureAtlases[brush.GetAtlasIndex()];
            if (atlas && atlas->HasTexture())
            {
                Lumina::Renderer2D::SetQuadTexture(atlas->GetTexture());
                Lumina::Renderer2D::SetQuadTextureCoords(brush.GetTextureCoords());
            }
        }

        Lumina::Renderer2D::DrawQuad();
    }

    void PanelViewport::RenderEraserPreview()
    {
        // Show a semi-transparent red square to indicate eraser
        Lumina::Renderer2D::SetQuadPosition(m_MouseFollowQuadPosition);
        Lumina::Renderer2D::SetQuadSize({ m_TileSize, m_TileSize });
        Lumina::Renderer2D::SetQuadRotation({ 0.0f, 0.0f, 0.0f });
        Lumina::Renderer2D::SetQuadTintColor({ 1.0f, 0.0f, 0.0f, 0.3f }); // Red, semi-transparent
        Lumina::Renderer2D::DrawQuad();
    }

    void PanelViewport::RenderFillPreview()
    {
        // Show a semi-transparent blue square to indicate fill
        Lumina::Renderer2D::SetQuadPosition(m_MouseFollowQuadPosition);
        Lumina::Renderer2D::SetQuadSize({ m_TileSize, m_TileSize });
        Lumina::Renderer2D::SetQuadRotation({ 0.0f, 0.0f, 0.0f });
        Lumina::Renderer2D::SetQuadTintColor({ 0.0f, 0.0f, 1.0f, 0.3f }); // Blue, semi-transparent
        Lumina::Renderer2D::DrawQuad();
    }

    void PanelViewport::RenderBasicHover()
    {
        // Show the basic green hover indicator
        Lumina::Renderer2D::SetQuadPosition(m_MouseFollowQuadPosition);
        Lumina::Renderer2D::SetQuadSize(m_MouseFollowQuadSize);
        Lumina::Renderer2D::SetQuadRotation({ 0.0f, 0.0f, 0.0f });
        Lumina::Renderer2D::SetQuadTintColor(m_MouseFollowQuadColor);
        Lumina::Renderer2D::DrawQuad();
    }

    void PanelViewport::HandleInput()
    {
        if (!ImGui::IsWindowHovered()) return;

        glm::ivec2 gridPos = GetGridPositionUnderMouse();
        if (!IsValidGridPosition(gridPos)) return;

        // Handle painting input
        if (Lumina::Input::IsMouseButtonPressed(Lumina::MouseCode::Left))
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
            m_Context->PaintTile(gridPos.x - 1, gridPos.y - 1); // Convert to 0-based coordinates
            break;
        case PaintingMode::Eraser:
            m_Context->EraseTile(gridPos.x - 1, gridPos.y - 1);
            break;
        case PaintingMode::Fill:
            m_Context->FillLayer(gridPos.x - 1, gridPos.y - 1);
            break;
        default:
            // No action for PaintingMode::None
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
        glm::vec3 cameraPosition = m_Camera->GetPosition();
        float zoom = m_Camera->GetZoom();

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