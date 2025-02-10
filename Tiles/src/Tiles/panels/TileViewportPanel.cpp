#include "TileViewportPanel.h"

#include <imgui.h>

#include "../TileLayer.h"

void TileViewportPanel::Render(TileLayer& layers, Lumina::TextureAtlas& atlas, int selectedTexture)
{
    ImGui::Begin("Scene");

    for (size_t y = 0; y < layers.LayerWidth(); y++)
    {
        for (size_t x = 0; x < layers.LayerHeight(); x++)
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

    for (size_t layer = 0; layer < layers.LayerSize(); layer++)
    {
        for (size_t y = 0; y < layers.LayerHeight(); y++)
        {
            for (size_t x = 0; x < layers.LayerWidth(); x++)
            {
                if (!layers.IsLayerVisible(layer))
                    continue;

                ImVec2 cursorPos = ImGui::GetCursorScreenPos();
                TileData& tile = layers.GetTile(layer, y, x);
                float offset = TILE_SIZE * m_Zoom;
                ImVec2 tileMin = ImVec2(cursorPos.x + x * offset, cursorPos.y + y * offset);
                ImVec2 tileMax = ImVec2(tileMin.x + offset, tileMin.y + offset);

                if (ImGui::IsMouseHoveringRect(tileMin, tileMax) && ImGui::IsMouseDown(0)) {
                    TileData previousTile = tile;

                    if (layers.GetActiveLayer() == layer)
                    {
                        if (selectedTexture >= 0)
                        {
                            TileAction action;

                            action.L = layer;
                            action.X = x;
                            action.Y = y;
                            action.Prev = tile;

                            if (m_Fill)
                            {
                                layers.FillLayer(selectedTexture, y, x);
                            }
                            else
                            {
                                tile.UseTexture = true;
                                tile.TextureIndex = selectedTexture;
                            }

                            if (m_Erase)
                            {
                                layers.ResetTile(y, x);
                            }

                            action.Curr = tile;

                            layers.RecordAction(action);
                        }
                    }
                }

                if (tile.UseTexture && tile.TextureIndex >= 0)
                {
                    intptr_t textureID = (intptr_t)atlas.GetTextureID();

                    glm::vec4 texCoords = atlas.GetTexCoords(static_cast<int>(tile.TextureIndex));
                    ImVec2 xy = ImVec2(texCoords.x, texCoords.y);
                    ImVec2 zw = ImVec2(texCoords.z, texCoords.w);

                    const ImU32 color = IM_COL32(255, 255, 255, 255);

                    ImGui::GetWindowDrawList()->AddImage((void*)textureID, tileMin, tileMax, xy, zw, color);
                }

                if (ImGui::IsMouseHoveringRect(tileMin, tileMax))
                {
                    layers.SetHoveredTile(y, x);
                    ImGui::GetWindowDrawList()->AddRect(tileMin, tileMax, IM_COL32(169, 169, 169, 255));
                }

            }
        }
    }

    ImGui::End();
}