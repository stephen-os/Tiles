#include "TileViewportPanel.h"

#include "../Core/Tools.h"

#include <algorithm>

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

    // Render checkerboard background
    for (float y = cursorPos.y; y < cursorPos.y + gridHeight; y += scaledSize)
    {
        for (float x = cursorPos.x; x < cursorPos.x + gridWidth; x += scaledSize)
        {
            ImVec2 minPos = ImVec2(x, y);
            ImVec2 maxPos = ImVec2(x + scaledSize, y + scaledSize);

            ImU32 fillColor = (((int)(x / scaledSize) + (int)(y / scaledSize)) % 2 == 0) ? CHECKERBOARD_COLOR_1 : CHECKERBOARD_COLOR_2;

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

                if (ImGui::IsMouseHoveringRect(tileMin, tileMax) && ImGui::IsMouseDown(0))
                {
                    if (l == m_Layers->GetActiveLayer())
                        HandleTileSelection(layer, tile, y, x);
                } 

                DrawTile(tile, tileMin, tileMax);

                if (ImGui::IsMouseHoveringRect(tileMin, tileMax))
                {
                    ImGui::GetWindowDrawList()->AddRect(tileMin, tileMax, SELECTION_BORDER_COLOR);
                }
            }
        }
    }
}

void TileViewportPanel::HandleTileSelection(Layer& layer, Tile& tile, size_t y, size_t x)
{
    if (m_Atlas->IsTextureSelected())
    {
        if (m_ToolModes->Fill)
            Tools::Fill(layer, m_Atlas->GetSelectedTexture(), y, x);
        else
            tile.SetTextureIndex(m_Atlas->GetSelectedTexture());

        if (m_ToolModes->Erase)
        {
            tile.Reset();
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