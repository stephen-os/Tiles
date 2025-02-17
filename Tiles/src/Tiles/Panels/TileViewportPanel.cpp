#include "TileViewportPanel.h"

#include "../Core/Tools.h"

#include <algorithm>
#include <iostream>

namespace Tiles
{

    void TileViewportPanel::Render()
    {
        if (!m_Layers) return;

        ImGui::Begin("Scene");

        HandleInput();
        RenderBackground();
        RenderTiles();

        ImGui::End();
    }

    void TileViewportPanel::RenderBackground()
    {
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        float gridWidth = m_Layers->GetWidth() * TILE_SIZE * m_Zoom;
        float gridHeight = m_Layers->GetHeight() * TILE_SIZE * m_Zoom;

        // Define checkerboard properties
        float scaledSize = CHECKERBOARD_SIZE * m_Zoom;
        int numCheckerCols = static_cast<int>(gridWidth / scaledSize);
        int numCheckerRows = static_cast<int>(gridHeight / scaledSize);

        // Render checkerboard background
        for (int row = 0; row < numCheckerRows; row++)
        {
            for (int col = 0; col < numCheckerCols; col++)
            {
                float x = cursorPos.x + col * scaledSize;
                float y = cursorPos.y + row * scaledSize;

                ImVec2 minPos = ImVec2(x, y);
                ImVec2 maxPos = ImVec2(std::min(x + scaledSize, cursorPos.x + gridWidth),
                    std::min(y + scaledSize, cursorPos.y + gridHeight));

                ImU32 fillColor = ((col + row) % 2 == 0) ? CHECKERBOARD_COLOR_1 : CHECKERBOARD_COLOR_2;

                ImGui::GetWindowDrawList()->AddRectFilled(minPos, maxPos, fillColor);
            }
        }

        // Render grid on top of checkerboard
        for (size_t y = 0; y < m_Layers->GetWidth(); y++)
        {
            for (size_t x = 0; x < m_Layers->GetHeight(); x++)
            {
                float offset = TILE_SIZE * m_Zoom;
                ImVec2 tileMin(cursorPos.x + x * offset, cursorPos.y + y * offset);
                ImVec2 tileMax(tileMin.x + offset, tileMin.y + offset);

                // Only render the grid outline, remove the filled background
                ImGui::GetWindowDrawList()->AddRect(tileMin, tileMax, OUTLINE_COLOR);
            }
        }
    }

    void TileViewportPanel::RenderTiles()
    {
        for (size_t l = 0; l < m_Layers->GetSize(); l++)
        {
            Layer& layer = m_Layers->GetLayer(l);
            if (!layer.GetVisibility()) continue;

            for (size_t y = 0; y < layer.GetHeight(); y++)
            {
                for (size_t x = 0; x < layer.GetWidth(); x++)
                {
                    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
                    float offset = TILE_SIZE * m_Zoom;
                    ImVec2 tileMin(cursorPos.x + x * offset, cursorPos.y + y * offset);
                    ImVec2 tileMax(tileMin.x + offset, tileMin.y + offset);

                    if (ImGui::IsMouseHoveringRect(tileMin, tileMax))
                    {
                        HandleSelection(l, y, x);
                        DrawHoveredTile(tileMin, tileMax, l, y, x);
                    }
                    DrawTile(tileMin, tileMax, l, y, x);
                }
            }
        }
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
            ImVec2 adjustedMin(tileMin.x + normalizedPos.x * TILE_SIZE * m_Zoom,
                tileMin.y + normalizedPos.y * TILE_SIZE * m_Zoom);
            ImVec2 adjustedMax(adjustedMin.x + TILE_SIZE * m_Zoom, adjustedMin.y + TILE_SIZE * m_Zoom);

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

    void TileViewportPanel::HandleInput()
    {
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
        {
            float scroll = ImGui::GetIO().MouseWheel;
            if (scroll != 0.0f)
            {
                float zoomFactor = 0.1f;
                m_Zoom = std::clamp(m_Zoom + scroll * zoomFactor, 0.5f, 3.0f);
            }
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

}