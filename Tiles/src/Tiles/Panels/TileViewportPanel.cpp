#include "TileViewportPanel.h"

#include "../Core/Tools.h"

#include <algorithm>

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
                    Tile& tile = layer.GetTile(y, x);
                    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
                    float offset = TILE_SIZE * m_Zoom;
                    ImVec2 tileMin(cursorPos.x + x * offset, cursorPos.y + y * offset);
                    ImVec2 tileMax(tileMin.x + offset, tileMin.y + offset);

                    // Only handle tile selection on mouse press, not hold
                    if (ImGui::IsMouseHoveringRect(tileMin, tileMax))
                    {
                        if (l == m_Layers->GetActiveLayer())
                        {
                            HandleTileSelection(layer, tile, y, x, tileMin);
                        }

                        // Highlight hovered tile
                        ImGui::GetWindowDrawList()->AddRect(tileMin, tileMax, SELECTION_BORDER_COLOR);
                    }

                    DrawTile(tile, tileMin, tileMax);
                }
            }
        }

        // Reset drag state when mouse is released
        if (!ImGui::IsMouseDown(0))
        {
            m_IsMouseDragging = false;
            m_LastMousePosition = ImVec2(-1, -1);
        }
    }


    void TileViewportPanel::HandleTileSelection(Layer& layer, Tile& tile, size_t y, size_t x, ImVec2 tilePos)
    {
        // Convert current tile position to ImVec2 for comparison
        ImVec2 currentTilePos(x, y);

        // Only process if mouse is just clicked or if we've moved to a new tile while dragging
        bool isNewClick = ImGui::IsMouseClicked(0) && !m_IsMouseDragging;
        bool isNewTileDuringDrag = ImGui::IsMouseDown(0) && m_IsMouseDragging &&
            (currentTilePos.x != m_LastMousePosition.x ||
                currentTilePos.y != m_LastMousePosition.y);

        if (isNewClick || isNewTileDuringDrag)
        {
            if (m_Atlas->IsTextureSelected())
            {
                if (m_ToolModes->Fill)
                {
                    // Fill tool should only trigger on new clicks, not during drag
                    if (isNewClick)
                    {
                        m_State->PushLayer(m_Layers->GetActiveLayer(), layer, StateType::Layer_Replace);
                        Tools::Fill(layer, m_Atlas->GetSelectedTexture(), y, x);
                    }
                }
                else
                {
                    m_State->PushTile(y, x, tile);
                    tile.SetTextureIndex(m_Atlas->GetSelectedTexture());
                }
            }

            if (m_ToolModes->Erase)
            {
                m_State->PushTile(y, x, tile);
                tile.Reset();
            }

            // Update tracking variables
            m_LastMousePosition = currentTilePos;
            if (isNewClick)
            {
                m_IsMouseDragging = true;
            }
        }
    }

    void TileViewportPanel::DrawTile(const Tile& tile, ImVec2 tileMin, ImVec2 tileMax)
    {
        if (!m_Atlas) return;

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

}