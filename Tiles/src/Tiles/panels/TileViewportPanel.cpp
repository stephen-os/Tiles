#include "TileViewportPanel.h"

#include <imgui.h>

#include "../TileLayer.h"

void TileViewportPanel::Render(int selectedTexture)
{
    ImGui::Begin("Scene");

    for (size_t y = 0; y < m_Layers->LayerWidth(); y++)
    {
        for (size_t x = 0; x < m_Layers->LayerHeight(); x++)
        {
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            float offset = TILE_SIZE * m_Zoom;
            ImVec2 tileMin = ImVec2(cursorPos.x + x * offset, cursorPos.y + y * offset);
            ImVec2 tileMax = ImVec2(tileMin.x + offset, tileMin.y + offset);

            const ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

            ImGui::GetWindowDrawList()->AddRectFilled(tileMin, tileMax, color);
            ImGui::GetWindowDrawList()->AddRect(tileMin, tileMax, IM_COL32(169, 169, 169, 255));
        }
    }

    for (size_t layer = 0; layer < m_Layers->LayerSize(); layer++)
    {
        for (size_t y = 0; y < m_Layers->LayerHeight(); y++)
        {
            for (size_t x = 0; x < m_Layers->LayerWidth(); x++)
            {
                if (!m_Layers->IsLayerVisible(layer))
                    continue;

                ImVec2 cursorPos = ImGui::GetCursorScreenPos();
                TileData& tile = m_Layers->GetTile(layer, y, x);
                float offset = TILE_SIZE * m_Zoom;
                ImVec2 tileMin = ImVec2(cursorPos.x + x * offset, cursorPos.y + y * offset);
                ImVec2 tileMax = ImVec2(tileMin.x + offset, tileMin.y + offset);

                if (ImGui::IsMouseHoveringRect(tileMin, tileMax) && ImGui::IsMouseDown(0)) {
                    TileData previousTile = tile;

                    if (m_Layers->GetActiveLayer() == layer)
                    {
                        if (selectedTexture >= 0)
                        {
                            TileAction action;

                            action.L = layer;
                            action.X = x;
                            action.Y = y;
                            action.Prev = tile;

                            if (m_Modes->Fill)
                            {
                                m_Layers->FillLayer(selectedTexture, y, x);
                            }
                            else
                            {
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

                if (tile.UseTexture && tile.TextureIndex >= 0)
                {
                    intptr_t textureID = (intptr_t)m_TextureAtlas->GetTextureID();

                    glm::vec4 texCoords = m_TextureAtlas->GetTexCoords(static_cast<int>(tile.TextureIndex));
                    ImVec2 xy = ImVec2(texCoords.x, texCoords.y);
                    ImVec2 zw = ImVec2(texCoords.z, texCoords.w);

                    const ImU32 color = IM_COL32(255, 255, 255, 255);

                    ImGui::GetWindowDrawList()->AddImage((void*)textureID, tileMin, tileMax, xy, zw, color);
                }

                if (ImGui::IsMouseHoveringRect(tileMin, tileMax))
                {
                    m_Layers->SetHoveredTile(y, x);
                    ImGui::GetWindowDrawList()->AddRect(tileMin, tileMax, IM_COL32(169, 169, 169, 255));
                }

            }
        }
    }

    ImGui::End();
}