#include "PanelViewport.h"
#include "Lumina/Core/Log.h"
#include "Lumina/Core/Input.h"
#include <algorithm>

namespace Tiles
{
    // Rendering constants
    namespace RenderSettings
    {
        static constexpr float DEFAULT_TILE_SIZE = 64.0f;
        static constexpr float MIN_ZOOM = 0.5f;
        static constexpr float MAX_ZOOM = 3.0f;
        static constexpr float ZOOM_SENSITIVITY = 0.01f;
        static constexpr float PAN_SENSITIVITY = 2.0f;
        static constexpr float GRID_Z_DEPTH = -1.0f;
        static constexpr float OUTLNE_Z_DEPTH = -0.5f;
        static constexpr float TILE_Z_DEPTH = 0.0f;
        static constexpr float OVERLAY_Z_DEPTH = 0.1f;
        static constexpr float HOVER_OUTLINE_THICKNESS = 2.0f;
    }

    // Input constants
    namespace InputSettings
    {
        static constexpr float CAMERA_MOVE_SPEED = 0.5f;
        static constexpr float MOUSE_PAN_SENSITIVITY = 0.002f;
    }

    // Color scheme
    namespace ColorScheme
    {
        static constexpr ImU32 GRID_COLOR = IM_COL32(80, 80, 80, 128);
        static constexpr ImU32 TILE_OUTLINE_COLOR = IM_COL32(255, 0, 0, 255); // this should probably be a glm::vec4
        static constexpr ImU32 HOVER_OUTLINE_COLOR = IM_COL32(255, 165, 0, 255);
        static constexpr ImU32 NO_PROJECT_COLOR = IM_COL32(179, 179, 179, 255);
    }

    // UI text
    namespace UIText
    {
        static constexpr const char* WINDOW_TITLE = "Viewport";
        static constexpr const char* NO_PROJECT_MESSAGE = "No project loaded";
    }

    PanelViewport::PanelViewport() : m_TileSize(RenderSettings::DEFAULT_TILE_SIZE)
    {
        m_RenderTarget = Renderer2D::CreateRenderTarget(512, 512);
	    
        // We currently cant do this with context because context gets set in OnUpdate. 
        // We will need some way of the context being known at construction time
        // Another option is creating some functionality for camera centering, which can get called the first time OnUpdate is called.  
        const float gridWidth = 16.0f; // m_Context->GetProject().GetLayerStack().GetWidth();
        const float gridHeight = 16.0f; // m_Context->GetProject().GetLayerStack().GetHeight();

        m_Camera.SetPosition({ m_TileSize * (gridWidth * 0.5f) + m_TileSize * 0.5f,m_TileSize * (gridHeight * 0.5f) + m_TileSize * 0.5f, 1.0f});

		m_Camera.SetZoom(1.0f);

        m_MouseFollowQuad.Size = { m_TileSize * 0.5f, m_TileSize * 0.5f }; // Half size
        m_MouseFollowQuad.TintColor = { 0.0f, 1.0f, 0.0f, 0.6f }; // Semi-transparent green
    }

    void PanelViewport::Render()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
        ImGui::Begin(UIText::WINDOW_TITLE, nullptr, flags);

        // Set members 
		m_MousePosition = ImGui::GetMousePos();
        
		// Set viewport position
		m_ViewportPosition = ImGui::GetCursorScreenPos();
		
		// Set viewport size
        m_ViewportSize = ImGui::GetContentRegionAvail();
        m_ViewportSize.x = std::max(m_ViewportSize.x, m_TileSize);
        m_ViewportSize.y = std::max(m_ViewportSize.y, m_TileSize);

		// Update camera based on viewport size
        m_Camera.SetSize(m_ViewportSize.x, m_ViewportSize.y);

		// Debug Circle at mouse position : ImGui side 
        ImDrawList* drawList = ImGui::GetForegroundDrawList();
        drawList->AddCircleFilled(m_MousePosition, 10.0f, IM_COL32(255, 255, 0, 200));
		
        
        {
			// Debug on renderer side
            auto worldPosition = ScreenToWorld();
            glm::vec3 cameraPos = m_Camera.GetPosition();
            m_MouseFollowQuad.Position = { worldPosition.x + cameraPos.x, worldPosition.y + cameraPos.y, 0.2f }; // Current (zoom)        
        }

        // Begin Rendering
        Renderer2D::SetRenderTarget(m_RenderTarget);
        Renderer2D::SetResolution(m_ViewportSize.x, m_ViewportSize.y);
        Renderer2D::Begin(m_Camera);

        // Render grid
        {
            glm::vec3 cameraPos = m_Camera.GetPosition();
            GridAttributes grid;
            grid.Position = { cameraPos.x + m_TileSize / 2, cameraPos.y + m_TileSize / 2, 0.0f };
            grid.Size = { 4096.0f, 4096.0f };
            grid.GridSize = m_TileSize / 2; // Half because grid lines are drawn at 2 tile intervals
            grid.Color = { 0.5f, 0.5f, 0.5f, 0.8f };
            grid.LineWidth = 2.0f;
            grid.ShowCheckerboard = true;
            grid.CheckerColor1 = { 0.27f, 0.27f, 0.27f, 1.0f };
            grid.CheckerColor2 = { 0.15f, 0.15f, 0.15f, 1.0f };
            Renderer2D::DrawGrid(grid);

            const float numTiles = 5.0f;

            const float gridWidth = m_Context->GetProject().GetLayerStack().GetWidth();
            const float gridHeight = m_Context->GetProject().GetLayerStack().GetHeight();

			const float offset = m_TileSize * 0.5f; 

			LineAttributes gridLine;
            gridLine.Color = { 1.0f, 0.0f, 0.0f , 1.0f };
			gridLine.Start = { offset + cameraPos.x, offset + cameraPos.y, 0.5f };
			gridLine.End = { m_TileSize * gridWidth + offset + cameraPos.x, offset + cameraPos.y, 0.5f };
			Renderer2D::DrawLine(gridLine);

            gridLine.Start = { offset + cameraPos.x, offset + cameraPos.y, 0.5f };
            gridLine.End = { offset + cameraPos.x, m_TileSize * gridHeight + offset + cameraPos.y, 0.5f };
			Renderer2D::DrawLine(gridLine);

			gridLine.Start = { m_TileSize * gridWidth + offset + cameraPos.x, m_TileSize * gridHeight + offset + cameraPos.y, 0.5f };
            gridLine.End = { offset + cameraPos.x, m_TileSize * gridHeight + offset + cameraPos.y, 0.5f };
			Renderer2D::DrawLine(gridLine);

            gridLine.Start = { m_TileSize * gridWidth + offset + cameraPos.x, m_TileSize * gridHeight + offset + cameraPos.y, 0.5f };
			gridLine.End = { m_TileSize * gridWidth + offset + cameraPos.x, offset + cameraPos.y, 0.5f };
			Renderer2D::DrawLine(gridLine);
        }
        
        if (ImGui::IsWindowHovered())
        {
            Renderer2D::DrawQuad(m_MouseFollowQuad);
        }

        if (ImGui::IsWindowHovered())
        {
            const float gridWidth = m_Context->GetProject().GetLayerStack().GetWidth();
            const float gridHeight = m_Context->GetProject().GetLayerStack().GetHeight();

			auto worldPosition = ScreenToWorld();
            glm::vec3 cameraPos = m_Camera.GetPosition();
            // Convert to grid coordinates
            glm::ivec2 gridPos = { (int)round(worldPosition.x / m_TileSize), (int)round(worldPosition.y / m_TileSize) };

            if (gridPos.x >= 1 && gridPos.x <= gridWidth && gridPos.y >= 1 && gridPos.y <= gridHeight)
            {
                // Move demo quad to the center of the clicked grid cell
                glm::vec2 gridCenter = { gridPos.x * m_TileSize, gridPos.y * m_TileSize };

                if (Input::IsMouseButtonPressed(MouseCode::Left))
                {
                    LUMINA_LOG_INFO("Painting Tile"); 
                    m_Context->PaintTile(gridPos.x, gridPos.y);
                }

                auto tile = m_Context->GetBrush(); 

                QuadAttributes previewQuad; 
                previewQuad.Position = { gridCenter.x + cameraPos.x, gridCenter.y + cameraPos.y, 0.1f };
                previewQuad.Rotation = tile.GetRotation();
                previewQuad.TintColor = tile.GetTint(); 
                auto scaler = tile.GetSize(); // May need to rename this to scaler or scale. 
                previewQuad.Size = { m_TileSize * scaler.x, m_TileSize * scaler.y };
                
                if (m_Context->GetProject().GetTextureAtlasCount() > 0)
                {
                    auto atlas = m_Context->GetProject().GetTextureAtlas(tile.GetAtlasIndex());
                    if (atlas->HasTexture())
                    {
                        previewQuad.Texture = atlas->GetTexture();
                        previewQuad.TextureCoords = tile.GetTextureCoords();
                    }

                }
                
                Renderer2D::DrawQuad(previewQuad);
            }
        }

        Renderer2D::End();
        Renderer2D::SetRenderTarget(nullptr);

        // Display the rendered texture
        ImGui::Image((void*)m_RenderTarget->GetTexture(), m_ViewportSize);

        m_MouseDelta = ImGui::GetIO().MouseWheel; 

        ImGui::End();
    }

    void PanelViewport::Update()
    {
        // Handle camera movement
        if (Input::IsKeyPressed(KeyCode::W))
			m_Camera.MoveUp(-InputSettings::CAMERA_MOVE_SPEED);
        if (Input::IsKeyPressed(KeyCode::S))
			m_Camera.MoveUp(InputSettings::CAMERA_MOVE_SPEED);
        if (Input::IsKeyPressed(KeyCode::A))
			m_Camera.MoveRight(InputSettings::CAMERA_MOVE_SPEED);
        if (Input::IsKeyPressed(KeyCode::D))
			m_Camera.MoveRight(-InputSettings::CAMERA_MOVE_SPEED);
        // Handle zoom

        if (m_MouseDelta != 0)
        {
            float zoomChange = m_MouseDelta * RenderSettings::ZOOM_SENSITIVITY;
            float newZoom = std::clamp(m_Camera.GetZoom() + zoomChange, RenderSettings::MIN_ZOOM, RenderSettings::MAX_ZOOM);
            m_Camera.SetZoom(newZoom);
        }
    }

    glm::vec2 PanelViewport::ScreenToWorld() const
    {
        glm::vec3 cameraPosition = m_Camera.GetPosition();
        float zoom = m_Camera.GetZoom();

        ImVec2 relativeMousePosition =
        {
            m_MousePosition.x - m_ViewportPosition.x,
            m_MousePosition.y - m_ViewportPosition.y
        }; 

        float centeredX = relativeMousePosition.x - m_ViewportSize.x / 2.0f;
        float centeredY = relativeMousePosition.y - m_ViewportSize.y / 2.0f;

        float worldX = cameraPosition.x + centeredX / zoom;
        float worldY = cameraPosition.y + centeredY / zoom;

		return { worldX, worldY };
	}
}