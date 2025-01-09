#include "TileEditor.h"

#include <string>
#include <iostream>
#include <queue>

#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>

#include "TileSerializer.h"

TileEditor::TileEditor() : m_ActiveLayer(0), m_SelectedTextureIndex(-1) {}

void TileEditor::Init(int width, int height)
{
    m_Atlas.CreateAtlas("res/texture/world_tileset.png", 16, 16);

    m_TileLayer.Init(width, height);
    m_TileLayer.LoadLayers("tiles.json");

    m_ActiveLayer = m_TileLayer.Size() - 1; 

    m_ActiveLayers.resize(m_TileLayer.Size(), true);
}

void TileEditor::Shutdown()
{
    // m_TileLayer.SaveLayers("tiles.json");
}

void TileEditor::Render()
{
    ImGui::Begin("Tile Editor", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar);

    if (ImGui::Button("Undo")) {
        m_TileLayer.UndoAction();
    }

    ImGui::SameLine();

    if (ImGui::Button("Redo")) {
        m_TileLayer.RedoAction();
    }

    ImGui::Separator();

    // Eraser tool.
    ImGui::Checkbox("Eraser Mode", &m_Modes.Erase);

    // FillLayer tool. 
    ImGui::Checkbox("Fill Mode", &m_Modes.Fill);

    ImGui::Separator();

    ImGui::Text("Layers:");

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::BeginChild("LayerBox", ImVec2(0, 200), true, ImGuiWindowFlags_None);

    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered]);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive]);

    for (int i = 0; i < m_TileLayer.Size(); ++i) {
        LayerData& layer = m_TileLayer.GetLayer(i);

        ImGui::PushID(i);

        bool flag = m_ActiveLayers[i];

        // Toggle visibility with customized checkbox
        ImGui::Checkbox("##Visible", &flag);
        ImGui::SameLine();
        m_ActiveLayers[i] = flag;

        // Layer selection
        if (ImGui::Selectable(layer.Name.c_str(), i == m_ActiveLayer)) 
        {
            m_ActiveLayer = i;
        }

        ImGui::PopID();
    }

    ImGui::PopStyleColor(2);
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::Separator();

    // Add Layer Button
    if (ImGui::Button("Add Layer"))
    {
        m_TileLayer.AddLayer("Layer " + std::to_string(m_TileLayer.Size() + 1));
        m_ActiveLayers.push_back(true);
    }

    ImGui::SameLine();

    // Delete Layer Button
    if (ImGui::Button("Delete Layer"))
    {
        m_TileLayer.DeleteLayer(m_ActiveLayer);
        m_ActiveLayers.erase(m_ActiveLayers.begin() + m_ActiveLayer);
        m_ActiveLayer = m_ActiveLayer - 1;
    }

    ImGui::SameLine();

    // Clear Layer Button
    if (ImGui::Button("Clear Layer"))
        m_TileLayer.ClearLayer(m_ActiveLayer);

    ImGui::Separator();

    // Editable layer name field
    LayerData& activeLayer = m_TileLayer.GetLayer(m_ActiveLayer);
    char layerNameBuffer[128];
    strncpy_s(layerNameBuffer, activeLayer.Name.c_str(), sizeof(layerNameBuffer) - 1);
    layerNameBuffer[sizeof(layerNameBuffer) - 1] = '\0';

    // ImGui input text for editable layer name
    if (ImGui::InputText("Layer Name", layerNameBuffer, sizeof(layerNameBuffer)))
    {
        activeLayer.Name = std::string(layerNameBuffer);
    }

    ImGui::Separator();

    ImGui::End();

    ImGui::Begin("Texture Selection");

    for (int y = 0; y < m_Atlas.GetGridHeight(); ++y) {
        for (int x = 0; x < m_Atlas.GetGridWidth(); ++x) {
            int index = y * m_Atlas.GetGridWidth() + x;
            glm::vec4 texCoords = m_Atlas.GetTexCoords(index); 

            float size = 30;
            ImVec2 buttonSize(size, size);
            ImVec2 xy = ImVec2(texCoords.x, texCoords.y);
            ImVec2 zw = ImVec2(texCoords.z, texCoords.w);

            ImGui::ImageButton(reinterpret_cast<void*>(static_cast<uintptr_t>(m_Atlas.GetTextureID())), buttonSize, xy, zw);

            if (index == m_SelectedTextureIndex)
            {
                ImVec2 min = ImGui::GetItemRectMin();
                ImVec2 max = ImGui::GetItemRectMax();
                ImGui::GetWindowDrawList()->AddRect(min, max, IM_COL32(169, 169, 169, 255), 3.0f, 0, 1.5f);
            }

            if (ImGui::IsItemClicked())
            {
                m_SelectedTextureIndex = index;
            }

            // 4 buttons per row
            if ((index + 1) % m_Atlas.GetGridWidth() != 0)
            {
                ImGui::SameLine();
            }
        }
    }

    ImGui::End();


    ImGui::Begin("Editor Layer");
    
    for (int layer = 0; layer < m_ActiveLayer; ++layer)
    {
        if (!m_ActiveLayers[layer]) continue;
        for (int y = 0; y < m_TileLayer.GetHeight(); y++)
        {
            for (int x = 0; x < m_TileLayer.GetWidth(); x++)
            {
                RenderTile(layer, y, x);
            }
        }
    }

    glm::vec3 hovered = { 0, 0, 0 };

    // Render tiles
    if (m_ActiveLayers[m_ActiveLayer])
    { 
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        for (int y = 0; y < m_TileLayer.GetHeight(); y++)
        {
            for (int x = 0; x < m_TileLayer.GetWidth(); x++)
            {
                ImGui::PushID(y * m_TileLayer.GetWidth() + x);

                // Note should Flip in TileLayer Class
                TileData& tile = m_TileLayer.GetTileData(m_ActiveLayer, y, x);
                float offset = m_Spec.TileSize;
                ImVec2 tileMin = ImVec2(cursorPos.x + x * offset, cursorPos.y + y * offset);
                ImVec2 tileMax = ImVec2(tileMin.x + m_Spec.TileSize, tileMin.y + m_Spec.TileSize);

                // Mouse interaction for painting
                if (ImGui::IsMouseHoveringRect(tileMin, tileMax) && ImGui::IsMouseDown(0)) {
                    TileData previousTile = tile;
                    tile.Opacity = 1.0f;

                    if (m_SelectedTextureIndex >= 0) 
                    {
                        if (m_Modes.Fill) 
                        {
                            m_TileLayer.FillLayer(m_SelectedTextureIndex, m_ActiveLayer, y, x);
                        }
                        else
                        {
                            tile.UseTexture = true;
                            tile.TextureIndex = m_SelectedTextureIndex;
                        }
                    }

                    if (m_Modes.Erase) 
                    {
                        m_TileLayer.ClearTile(m_ActiveLayer, y, x);
                    }

                    m_TileLayer.RecordAction(m_ActiveLayer, y, x);
                }

                RenderTile(m_ActiveLayer, y, x); 

                if (ImGui::IsMouseHoveringRect(tileMin, tileMax))
                {
                    hovered = { m_ActiveLayer, y, x };
                    ImGui::GetWindowDrawList()->AddRect(tileMin, tileMax, IM_COL32(169, 169, 169, 255));
                }

                ImGui::PopID();
            }
        }
    }

    ImGui::End();

    ImGui::Begin("Tile Attributes");

    TileData& hoveredTile = m_TileLayer.GetTileData(hovered.x, hovered.y, hovered.z);

    ImGui::Text("Active Layer: %d", m_ActiveLayer);
    ImGui::Text("Tile Position: (%d, %d)", hovered.y, hovered.z);
    ImGui::Text("Texture Index: %d", hoveredTile.TextureIndex);
    ImGui::Text("Opacity: %.2f", hoveredTile.Opacity);

    ImGui::End();
}

void TileEditor::RenderTile(int index, int y, int x)
{
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    ImGui::PushID(index * m_TileLayer.GetWidth() * m_TileLayer.GetHeight() + y * m_TileLayer.GetWidth() + x);

    TileData& tile = m_TileLayer.GetTileData(index, y, x);
    float offset = m_Spec.TileSize;
    ImVec2 tileMin = ImVec2(cursorPos.x + x * offset, cursorPos.y + y * offset);
    ImVec2 tileMax = ImVec2(tileMin.x + m_Spec.TileSize, tileMin.y + m_Spec.TileSize);

    // Draw the tile
    if (tile.UseTexture && tile.TextureIndex >= 0)
    {
        intptr_t textureID = (intptr_t)m_Atlas.GetTextureID();

        glm::vec4 texCoords = m_Atlas.GetTexCoords(tile.TextureIndex);
        ImVec2 xy = ImVec2(texCoords.x, texCoords.y);
        ImVec2 zw = ImVec2(texCoords.z, texCoords.w);

        const int r = static_cast<int>(tile.Opacity * 255);
        const int g = static_cast<int>(tile.Opacity * 255);
        const int b = static_cast<int>(tile.Opacity * 255);
        const int a = 255;

        const ImU32 color = IM_COL32(r, g, b, a);

        ImGui::GetWindowDrawList()->AddImage((void*)textureID, tileMin, tileMax, xy, zw, color);
    }

    ImGui::PopID();

}