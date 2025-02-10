#include "AttributePanel.h"

#include "imgui.h"

void AttributePanel::Render()
{
    if (!m_TileLayer || !m_TextureAtlas) return; // Ensure pointers are valid before rendering

    ImGui::Begin("Tile Attributes");

    TileData& tile = m_TileLayer->GetHoveredTile();

    ImGui::Text("Use Texture: %s", tile.UseTexture ? "Yes" : "No");

    if (tile.UseTexture && tile.TextureIndex >= 0)
    {
        intptr_t textureID = (intptr_t)m_TextureAtlas->GetTextureID();
        glm::vec4 texCoords = m_TextureAtlas->GetTexCoords(static_cast<int>(tile.TextureIndex));

        ImVec2 uvMin(texCoords.x, texCoords.y);
        ImVec2 uvMax(texCoords.z, texCoords.w);

        ImVec2 imageSize(64.0f, 64.0f);
        ImVec2 availableRegion = ImGui::GetContentRegionAvail();
        ImVec2 center((availableRegion.x - imageSize.x) * 0.5f, (availableRegion.y - imageSize.y) * 0.5f);

        ImGui::SetCursorPosX(center.x);
        ImGui::Image((void*)textureID, imageSize, uvMin, uvMax);
    }

    ImGui::End();
}
