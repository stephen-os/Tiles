#include "TileViewportPanel.h"

#include "../Core/Tools.h"

#include "Lumina/Utils/FileReader.h"

#include <algorithm>
#include <array>
#include <iostream>

namespace Tiles
{

    TileViewportPanel::TileViewportPanel()
    {
        // Data
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
    }

    void TileViewportPanel::Render()
    {
        if (!m_Layers) return;

        ImGui::Begin("Scene");

        HandleMouseInput();
        RenderBackground();
        RenderTiles();

        ImGui::End();
    }

    void TileViewportPanel::RenderBackground()
    {
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 mousePos = ImGui::GetMousePos();

        m_Renderer.Begin();
        m_Renderer.OnWindowResize(viewportSize.x, viewportSize.y);

        m_Renderer.Clear();
        m_Renderer.ClearColor(0.1f, 0.1f, 0.1f);
        m_Renderer.Enable(Lumina::RenderState::DepthTest);
        m_Renderer.Enable(Lumina::RenderState::CullFace);

        m_BackgroundShader->Bind();

        glm::vec2 gridSize = { 20, 15 };

        // Update uniforms with the new view matrix
        m_BackgroundShader->SetUniformMatrix4fv("u_ViewProjection", m_Camera.GetViewMatrix());
        m_BackgroundShader->SetUniform1f("u_AspectRatio", viewportSize.x / viewportSize.y);
        m_BackgroundShader->SetUniform1f("u_GridSpacing", 0.01f);
        m_BackgroundShader->SetUniform2fv("u_GridSize", { m_Layers->GetWidth() * 4.0f, m_Atlas->GetHeight() * 4.0f });
        m_BackgroundShader->SetUniform3fv("u_GridColor1", { 0.47f, 0.47f, 0.47f });
        m_BackgroundShader->SetUniform3fv("u_GridColor2", { 0.31f, 0.31f, 0.31f });

        m_Renderer.Draw(m_Background);

        ImGui::Image((void*)(intptr_t)m_Renderer.GetID(), viewportSize);
        m_Renderer.End();
    }

    void TileViewportPanel::RenderTiles()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f); // Remove roundness

        // Set transparent colors for the button fill
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));        // Fully transparent button
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.0f, 0.0f)); // Transparent when hovered
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));  // Transparent when clicked

        // Set border colors and thickness
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));        // Transparent border by default
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));  // Transparent border shadow
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);                       // Border thickness


        for (size_t l = 0; l < m_Layers->GetSize(); l++)
        {
            Layer& layer = m_Layers->GetLayer(l);
            if (!layer.GetVisibility()) continue;

            glm::vec2 cameraPos = m_Camera.GetPosition();

            for (size_t y = 0; y < layer.GetHeight(); y++)
            {
                for (size_t x = 0; x < layer.GetWidth(); x++)
                {
                    ImVec2 buttonSize = ImVec2(TILE_SIZE * m_Camera.GetZoom(), TILE_SIZE * m_Camera.GetZoom());
                    ImGui::SetCursorPos(ImVec2(8 + (1000 * cameraPos.x) + (x * buttonSize.x), (30 + 1000 * cameraPos.y) + (y * buttonSize.y)));
                    
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

        // Restore previous styles
        ImGui::PopStyleVar(2);  // Pop FrameRounding and FrameBorderSize
        ImGui::PopStyleColor(5); // Pop all color styles we pushed
    }

    void TileViewportPanel::HandleSelection(size_t l, size_t y, size_t x)
    {
        if (!m_Selection || m_Selection->Empty()) 
            return;

        if (l != m_Layers->GetActiveLayer())
            return;
        
        ImVec2 currentTilePos(y, x);
        if (!IsNewClick() && !IsNewTileDuringDrag(currentTilePos))
			return;

        // std::cout << "we getting to here?" << std::endl;

        // Are we dragging?
        m_LastMousePosition = currentTilePos;
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

    void TileViewportPanel::DrawHoveredTile(ImVec2 tileMin, ImVec2 tileMax, size_t l, size_t y, size_t x)
    {
        if (!m_Atlas || !m_Selection || m_Selection->Empty()) return;

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

            // Adjust tile position based on normalized offset
            ImVec2 adjustedMin(tileMin.x + normalizedPos.x * TILE_SIZE * m_Camera.GetZoom(),
                tileMin.y + normalizedPos.y * TILE_SIZE * m_Camera.GetZoom());
            ImVec2 adjustedMax(adjustedMin.x + TILE_SIZE * m_Camera.GetZoom(), adjustedMin.y + TILE_SIZE * m_Camera.GetZoom());

            // Texture UV mapping
            ImVec2 uvMin(texCoords.x, texCoords.y);
            ImVec2 uvMax(texCoords.z, texCoords.w);

            // Draw the texture
            ImGui::GetWindowDrawList()->AddImage((void*)textureID, adjustedMin, adjustedMax, uvMin, uvMax, FILL_COLOR);
            ImGui::GetWindowDrawList()->AddRect(tileMin, tileMax, SELECTION_BORDER_COLOR);
        }
    }

    void TileViewportPanel::DrawTile(ImVec2 tileMin, ImVec2 tileMax, size_t l, size_t y, size_t x)
    {
        if (!m_Atlas->IsCreated()) 
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

    bool TileViewportPanel::IsNewClick()
    {
        return ImGui::IsMouseClicked(0) && !m_IsMouseDragging;
    }

    bool TileViewportPanel::IsNewTileDuringDrag(ImVec2 currentTilePos)
    {
        return ImGui::IsMouseDown(0) && m_IsMouseDragging && 
            (currentTilePos.x != m_LastMousePosition.x || currentTilePos.y != m_LastMousePosition.y);
    }

    void TileViewportPanel::HandleMouseInput()
    {
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 mousePos = ImGui::GetMousePos();

        // Check if mouse is within viewport bounds
        if (!IsMouseInViewport(mousePos, windowPos, windowSize))
            return;

        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
        {
            if (!m_IsMiddleMouseDown)
            {
                // Middle mouse just pressed
                m_IsMiddleMouseDown = true;
                m_LastMousePos = glm::vec2(mousePos.x, mousePos.y);
            }
            else
            {
                // Middle mouse being held - calculate drag delta
                glm::vec2 currentMousePos(mousePos.x, mousePos.y);
                glm::vec2 mouseDelta = currentMousePos - m_LastMousePos;

                m_Camera.Drag(mouseDelta);

                m_AnchorePos = m_Camera.GetPosition();

                m_LastMousePos = currentMousePos;
            }
        }
        else
        {
            m_IsMiddleMouseDown = false;
        }

        // Handle zoom input
        float mouseWheel = ImGui::GetIO().MouseWheel;
        if (mouseWheel != 0.0f)
        {
            m_Camera.Zoom(mouseWheel);
        }
    }

    bool TileViewportPanel::IsMouseInViewport(const ImVec2& mousePos, const ImVec2& windowPos, const ImVec2& windowSize)
    {
        return (mousePos.x >= windowPos.x && mousePos.x <= windowPos.x + windowSize.x &&
            mousePos.y >= windowPos.y && mousePos.y <= windowPos.y + windowSize.y);
    }
}