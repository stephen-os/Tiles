#include "ViewportPanel.h"

#include "../Core/Tools.h"

#include "Lumina/Utils/FileReader.h"

#include <algorithm>
#include <array>

#include <iostream>

namespace Tiles
{

    ViewportPanel::ViewportPanel()
    {
        // This probably should be handled by some Tile or Quad class. 
        // Not very high on the priorty list right now. 
        float vertices[] = 
        {
            // Positions    
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f
        };

        uint32_t indices[] = 
        {
            0, 1, 2,
            2, 3, 0
        };

        // Buffer
        m_Background = Lumina::CreateRef<Lumina::VertexArray>();

        auto vertexBuffer = Lumina::CreateRef<Lumina::VertexBuffer>(vertices, sizeof(vertices));
        auto indexBuffer = Lumina::CreateRef<Lumina::IndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));

        vertexBuffer->SetLayout({{ Lumina::BufferDataType::Float3, "a_Position" }});

        m_Background->AddVertexBuffer(vertexBuffer);
		m_Background->SetIndexBuffer(indexBuffer);

        // Shader
        const std::string vertexSrc = Lumina::ReadFile("res/shaders/GridShader.vert");
        const std::string fragmentSrc = Lumina::ReadFile("res/shaders/GridShader.frag");

        m_BackgroundShader = Lumina::CreateRef<Lumina::ShaderProgram>(vertexSrc, fragmentSrc);

        m_Renderer.Init(); 

        m_Camera.Drag({ 40.0f, 56.0f });
    }

    void ViewportPanel::OnUIRender()
    {
        ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::SetCursorPos({ 0.0f, 0.0f });

        HandleMouseInput();
        RenderBackground();
        RenderTiles();

        ImGui::End();
    }

    void ViewportPanel::RenderBackground()
    {
        ImVec2 viewportSize = { 1000, 1000 };
     
        m_Renderer.Begin();
        m_Renderer.OnWindowResize(m_ViewportSize.x, m_ViewportSize.y);

        m_Renderer.Clear();

        m_BackgroundShader->Bind();

        m_BackgroundShader->SetUniformMatrix4fv("u_ViewProjection", m_Camera.GetViewMatrix());
        m_BackgroundShader->SetUniform1f("u_AspectRatio", m_ViewportSize.x / m_ViewportSize.y);
        m_BackgroundShader->SetUniform2fv("u_GridSize", { m_Layers->GetWidth() * 4.0f, m_Layers->GetHeight() * 4.0f });

        m_Renderer.Draw(m_Background);

        ImGui::Image((void*)(intptr_t)m_Renderer.GetID(), ImVec2(m_ViewportSize.x, m_ViewportSize.y));

        m_Renderer.End();
    }

    void ViewportPanel::RenderTiles()
    {
        if (!m_Layers || m_Layers->IsEmpty()) 
            return;

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));         // Fully transparent button
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));  // Transparent when hovered
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));   // Transparent when clicked
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));         // Transparent border by default
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));   // Transparent border shadow
        
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);                         // Remove roundness
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);                       // Border thickness

        for (size_t l = 0; l < m_Layers->GetSize(); l++)
        {
            Layer& layer = m_Layers->GetLayer(l);
            if (!layer.GetVisibility()) continue;

            glm::vec2 cameraPos = m_Camera.GetPosition() * m_ViewportSize;

            for (size_t y = 0; y < layer.GetHeight(); y++)
            {
                for (size_t x = 0; x < layer.GetWidth(); x++)
                {
                    float scale = m_ViewportSize.x / 1000;
                    ImVec2 buttonSize = ImVec2(m_TileSize * m_Camera.GetZoom() * scale, m_TileSize * m_Camera.GetZoom() * scale);
                    ImGui::SetCursorPos(ImVec2(cameraPos.x + (x * buttonSize.x), cameraPos.y + (y * buttonSize.y)));
                    
                    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
                    ImVec2 tileMin(cursorPos.x, cursorPos.y);
                    ImVec2 tileMax(tileMin.x + buttonSize.x, tileMin.y + buttonSize.y);

                    if (ImGui::IsMouseHoveringRect(tileMin, tileMax))
                    {
                        HandleSelection(l, y, x);
                        DrawHoveredTile(tileMin, tileMax, l, y, x);
                    }
                    DrawTile(tileMin, tileMax, l, y, x);
                }
            }
        }

        ImGui::PopStyleColor(5);
        ImGui::PopStyleVar(2);
    }

    void ViewportPanel::HandleSelection(size_t l, size_t y, size_t x)
    {
        // Is there a selection?
        if (!m_Selection || m_Selection->Empty()) 
            return;

        // Are we ont the active layer? 
        if (l != m_Layers->GetActiveLayer())
            return;

        // Is this a new click?
        glm::vec2 currentTilePos(y, x);
        if (!IsNewClick() && !IsNewTileDuringDrag(currentTilePos))
			return;

        // Are we dragging?
        m_LastMousePos = currentTilePos;
        if (IsNewClick())
            m_IsMouseDragging = true; 
         
        // If we are erasing, that is all we will do in this method. 
        if (m_ToolModes->Erase)
        {
            Tile& tile = m_Layers->GetTile(l, y, x);
            m_State->PushTile(y, x, tile);
            tile.Reset();
            return;
        }

        // For now we are only going to fill what is the first texture in the selection
        // otherwise paint with whole selection. 
        if (m_ToolModes->Fill)
        {
            Layer& layer = m_Layers->GetLayer(l);
            m_State->PushLayer(m_Selection->Front(), layer, StateType::Layer_Replace);
            Tools::Fill(layer, m_Selection->Front(), y, x);
        }
        else
        {
            int baseIndex = m_Selection->Front();
            glm::vec2 basePos = m_Atlas->GetPosition(baseIndex);

            for (int texture : *m_Selection)
            {
                glm::vec2 relativePos = m_Atlas->GetPosition(texture);
                glm::vec2 normalizedPos = relativePos - basePos;

                int targetX = x + (int)normalizedPos.x;
                int targetY = y + (int)normalizedPos.y;

                // Skip out-of-bounds tiles
                if (targetX < 0 || targetY < 0 || targetX >= m_Layers->GetWidth() || targetY >= m_Layers->GetHeight())
                    continue;

                // Get the tile
                Tile& tile = m_Layers->GetTile(l, targetY, targetX);
                
                // Store previouse state
                m_State->PushTile(targetY, targetX, tile);

                // Update texture
                tile.SetTextureIndex(texture);
            }
        }
    }

    void ViewportPanel::DrawHoveredTile(ImVec2 tileMin, ImVec2 tileMax, size_t l, size_t y, size_t x)
    {
        if (!m_Atlas || !m_Atlas->IsCreated())
			return;

        if (!m_Selection || m_Selection->Empty()) 
            return;

        // We dont draw the hovered tile if we are erasing
        if (m_ToolModes->Erase)
            return; 

        intptr_t textureID = (intptr_t)m_Atlas->GetTextureID();
        int baseIndex = m_Selection->Front();

        // Get base (reference) position
        glm::vec2 basePos = m_Atlas->GetPosition(baseIndex);

        for (int texture : *m_Selection)
        {
            glm::vec2 relativePos = m_Atlas->GetPosition(texture);
            glm::vec4 texCoords = m_Atlas->GetTexCoords(texture);

            // Translate relative position so the first selected tile is at (0,0)
            glm::vec2 normalizedPos = relativePos - basePos;

            // Adjust tile position based on normalized offset and scale 
            float scale = m_ViewportSize.x / 1000;
            ImVec2 adjustedMin(tileMin.x + normalizedPos.x * m_TileSize * m_Camera.GetZoom() * scale, tileMin.y + normalizedPos.y * m_TileSize * m_Camera.GetZoom() * scale);
            ImVec2 adjustedMax(adjustedMin.x + m_TileSize * m_Camera.GetZoom() * scale, adjustedMin.y + m_TileSize * m_Camera.GetZoom() * scale);

            // Texture UV mapping
            ImVec2 uvMin(texCoords.x, texCoords.y);
            ImVec2 uvMax(texCoords.z, texCoords.w);

            // Draw the texture
            ImGui::GetWindowDrawList()->AddImage((void*)textureID, adjustedMin, adjustedMax, uvMin, uvMax, FILL_COLOR);
            ImGui::GetWindowDrawList()->AddRect(tileMin, tileMax, SELECTION_BORDER_COLOR);
        }
    }

    void ViewportPanel::DrawTile(ImVec2 tileMin, ImVec2 tileMax, size_t l, size_t y, size_t x)
    {
        if (!m_Atlas || !m_Atlas->IsCreated())
            return;

        Tile& tile = m_Layers->GetTile(l, y, x);

        if (tile.UseTexture())
        {
            intptr_t textureID = (intptr_t)m_Atlas->GetTextureID();
            glm::vec4 texCoords = m_Atlas->GetTexCoords(tile.GetTextureIndex());
            ImVec2 uvMin(texCoords.x, texCoords.y);
            ImVec2 uvMax(texCoords.z, texCoords.w);

            ImGui::GetWindowDrawList()->AddImage((void*)textureID, tileMin, tileMax, uvMin, uvMax, FILL_COLOR);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // Utils
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ViewportPanel::IsNewClick()
    {
        return ImGui::IsMouseClicked(0) && !m_IsMouseDragging;
    }

    bool ViewportPanel::IsNewTileDuringDrag(glm::vec2 currentTilePos)
    {
        return ImGui::IsMouseDown(0) && m_IsMouseDragging && 
            (currentTilePos.x != m_LastMousePos.x || currentTilePos.y != m_LastMousePos.y);
    }

    void ViewportPanel::HandleMouseInput()
    {
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 mousePos = ImGui::GetMousePos();

        if (!IsMouseInViewport(mousePos, windowPos, windowSize))
            return;

        // Translating Screen
        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
        {
            if (!m_IsMiddleMouseDown)
            {
                m_IsMiddleMouseDown = true;
                m_LastMousePos = glm::vec2(mousePos.x, mousePos.y);
            }
            else
            {
                glm::vec2 currentMousePos(mousePos.x, mousePos.y);
                glm::vec2 mouseDelta = currentMousePos - m_LastMousePos;

                m_Camera.Drag(mouseDelta);
                m_LastMousePos = currentMousePos;
            }
        }
        else
        {
            m_IsMiddleMouseDown = false;
        }

        // Zooming
        float scrollDelta = ImGui::GetIO().MouseWheel;
        if (scrollDelta != 0.0f)
            m_Camera.Zoom(scrollDelta);
    }

    bool ViewportPanel::IsMouseInViewport(const ImVec2& mousePos, const ImVec2& windowPos, const ImVec2& windowSize)
    {
        return (mousePos.x >= windowPos.x && mousePos.x <= windowPos.x + windowSize.x &&
            mousePos.y >= windowPos.y && mousePos.y <= windowPos.y + windowSize.y);
    }
}