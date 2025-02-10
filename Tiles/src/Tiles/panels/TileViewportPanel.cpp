#include "TileViewportPanel.h"

#include <algorithm>

void TileViewportPanel::Render(int selectedTexture) {
    ImGui::Begin("Scene");

    if (!m_Layers) return;

    HandleScrolling();

    RenderTileGrid();
    RenderLayerTiles(selectedTexture);

    ImGui::End();
}

void TileViewportPanel::RenderTileGrid()
{
    for (size_t y = 0; y < m_Layers->LayerHeight(); y++)
    {
        for (size_t x = 0; x < m_Layers->LayerWidth(); x++)
        {
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            float offset = TILE_SIZE * m_Zoom;
            ImVec2 tileMin(cursorPos.x + x * offset, cursorPos.y + y * offset);
            ImVec2 tileMax(tileMin.x + offset, tileMin.y + offset);

            ImGui::GetWindowDrawList()->AddRectFilled(tileMin, tileMax, BACKGROUND_COLOR);
            ImGui::GetWindowDrawList()->AddRect(tileMin, tileMax, OUTLINE_COLOR);
        }
    }
}

void TileViewportPanel::RenderLayerTiles(int selectedTexture)
{
    for (size_t layer = 0; layer < m_Layers->LayerSize(); layer++)
    {
        if (!m_Layers->IsLayerVisible(layer)) continue;

        for (size_t y = 0; y < m_Layers->LayerHeight(); y++)
        {
            for (size_t x = 0; x < m_Layers->LayerWidth(); x++)
            {
                TileData& tile = m_Layers->GetTile(layer, y, x);
                ImVec2 cursorPos = ImGui::GetCursorScreenPos();
                float offset = TILE_SIZE * m_Zoom;
                ImVec2 tileMin(cursorPos.x + x * offset, cursorPos.y + y * offset);
                ImVec2 tileMax(tileMin.x + offset, tileMin.y + offset);

                if (ImGui::IsMouseHoveringRect(tileMin, tileMax) && ImGui::IsMouseDown(0))
                    HandleTileSelection(layer, x, y, selectedTexture);

                if (tile.UseTexture && tile.TextureIndex >= 0)
                    DrawTileTexture(tile, tileMin, tileMax);

                if (ImGui::IsMouseHoveringRect(tileMin, tileMax))
                {
                    m_Layers->SetHoveredTile(y, x);
                    ImGui::GetWindowDrawList()->AddRect(tileMin, tileMax, SELECTION_BORDER_COLOR);
                }
            }
        }
    }
}

void TileViewportPanel::HandleTileSelection(int layer, int x, int y, int selectedTexture)
{
    TileData& tile = m_Layers->GetTile(layer, y, x);
    TileAction action{ layer, x, y, tile };

    if (m_Layers->GetActiveLayer() == layer) {
        if (selectedTexture >= 0) {
            if (m_Modes->Fill)
            {
                m_Layers->FillLayer(selectedTexture, y, x);
            }
            else {
                tile.UseTexture = true;
                tile.TextureIndex = selectedTexture;
            }

            if (m_Modes->Erase)
            {
                m_Layers->ResetTile(y, x);
            }

            action.Curr = tile;
            m_Layers->RecordAction(action);
        }
    }
}

void TileViewportPanel::DrawTileTexture(const TileData& tile, ImVec2 tileMin, ImVec2 tileMax) {
    if (!m_TextureAtlas) return;

    intptr_t textureID = (intptr_t)m_TextureAtlas->GetTextureID();
    glm::vec4 texCoords = m_TextureAtlas->GetTexCoords(static_cast<int>(tile.TextureIndex));
    ImVec2 uvMin(texCoords.x, texCoords.y);
    ImVec2 uvMax(texCoords.z, texCoords.w);

    ImGui::GetWindowDrawList()->AddImage((void*)textureID, tileMin, tileMax, uvMin, uvMax, FILL_COLOR);
}

void TileViewportPanel::HandleScrolling()
{
    float scroll = ImGui::GetIO().MouseWheel;
    if (scroll != 0.0f) {
        float zoomFactor = 0.1f;
        m_Zoom = std::clamp(m_Zoom + scroll * zoomFactor, 0.5f, 3.0f);
    }
}