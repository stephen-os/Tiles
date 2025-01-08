#include "TileEditor.h"

#include <string>
#include <iostream>
#include <queue>

#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>

#include "TileSerializer.h"

TileEditor::TileEditor() : 
    m_ActiveLayer(0), m_Padding(0.0f), m_TileSize(40.0f),
    m_EraserMode(false), m_FillMode(false), m_Opacity(1.0f), 
    m_SelectedTextureIndex(-1), m_CurrentScenePath("res/maps/tiles.json"),
    m_TileAtlasPath("res/texture/world_tileset.png") {}

void TileEditor::Init(int width, int height)
{
    m_TileLayer.Init(width, height);
    m_TileLayer.LoadLayers("tiles.json");

    m_ActiveLayer = m_TileLayer.Size() - 1; 

    LoadTextures(); 
}

void TileEditor::Shutdown()
{
    // m_TileLayer.SaveLayers("tiles.json");
}

void TileEditor::LoadTextures()
{
    // Need a check here to check if there is a texture loaded. 
    if (!m_TileAtlasPath.empty())
    {
        m_Atlas.CreateAtlas(m_TileAtlasPath, 16, 16);
    }
    else
    {
        std::cerr << "Tile atlas path is empty!" << std::endl;
    }
}

void TileEditor::Render()
{
    ImGui::Begin("Tile Editor", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar);
#if 0
    // Scene management
    ImGui::Text("Scene Management:");
    static char scenePathBuffer[256];
    strncpy_s(scenePathBuffer, m_CurrentScenePath.c_str(), sizeof(scenePathBuffer) - 1);
    scenePathBuffer[sizeof(scenePathBuffer) - 1] = '\0';

    if (ImGui::InputText("Scene Path", scenePathBuffer, sizeof(scenePathBuffer))) {
        m_CurrentScenePath = std::string(scenePathBuffer);
    }

    if (ImGui::Button("Load Scene")) {
        m_TileLayers = TileSerializer::Deserialize(m_CurrentScenePath);
        if (m_TileLayers.size() == 0) {
            AddLayer();
        }
        else {
            m_NumLayers = (int)m_TileLayers.size();
        }
        
    }

    ImGui::SameLine();

    if (ImGui::Button("Save Scene")) {
        TileSerializer::Serialize(m_TileLayers, m_CurrentScenePath);
    }

    ImGui::Separator();
#endif

    ImGui::Text("Tile Atlas Path:");
    static char atlasPathBuffer[256];
    strncpy_s(atlasPathBuffer, m_TileAtlasPath.c_str(), sizeof(atlasPathBuffer) - 1);
    atlasPathBuffer[sizeof(atlasPathBuffer) - 1] = '\0';

    if (ImGui::InputText("Atlas Path", atlasPathBuffer, sizeof(atlasPathBuffer))) {
        m_TileAtlasPath = std::string(atlasPathBuffer);
    }

    if (ImGui::Button("Reload Atlas")) {
        LoadTextures();
    }

    ImGui::Separator();

    if (ImGui::Button("Undo")) {
        m_TileLayer.UndoAction();
    }

    ImGui::SameLine();

    if (ImGui::Button("Redo")) {
        m_TileLayer.RedoAction();
    }

    ImGui::Separator();

    // Eraser tool.
    ImGui::Checkbox("Eraser Mode", &m_EraserMode);

    // FillLayer tool. 
    ImGui::Checkbox("Fill Mode", &m_FillMode);

    // Texture selection buttons
    ImGui::Text("Textures:");
    
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();

    ImGui::BeginChild("TextureChild", ImVec2(0, viewportSize.y / 4), true, ImGuiWindowFlags_HorizontalScrollbar);

    for (int y = 0; y < m_Atlas.GetGridHeight(); ++y) {
        for (int x = 0; x < m_Atlas.GetGridWidth(); ++x) {
            int index = y * m_Atlas.GetGridWidth() + x;
            glm::vec4 texCoords = m_Atlas.GetTexCoords(index); 

            float size = (viewportSize.x - 310) / m_Atlas.GetGridWidth();
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

    ImGui::EndChild();

    // Opacity slider
    ImGui::SliderFloat("Opacity", &m_Opacity, 0.0f, 1.0f);

    // Layer selector and management
    ImGui::Text("Layers:");
    std::vector<const char*> layerNames;
    layerNames.reserve(m_TileLayer.Size());
    for (const auto& layer : m_TileLayer) {
        layerNames.push_back(layer.Name.c_str());
    }
    ImGui::Combo("Active Layer", &m_ActiveLayer, layerNames.data(), static_cast<int>(layerNames.size()));

    // Add Layer Button
    if (ImGui::Button("Add Layer"))
        m_TileLayer.AddLayer("Layer " + std::to_string(m_TileLayer.Size() + 1));

    ImGui::SameLine();

    // Delete Layer Button
    if (ImGui::Button("Delete Layer"))
        m_TileLayer.DeleteLayer(m_ActiveLayer);

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

    ImGui::Begin("Editor Layer");
    
    for (int layer = 0; layer < m_ActiveLayer; ++layer)
    {
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
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    for (int y = 0; y < m_TileLayer.GetHeight(); y++)
    {
        for (int x = 0; x < m_TileLayer.GetWidth(); x++)
        {
            ImGui::PushID(y * m_TileLayer.GetWidth() + x);

            // Note should Flip in TileLayer Class
            TileData& tile = m_TileLayer.GetTileData(m_ActiveLayer, y, x);
            float offset = m_TileSize + m_Padding;
            ImVec2 tileMin = ImVec2(cursorPos.x + x * offset, cursorPos.y + y * offset);
            ImVec2 tileMax = ImVec2(tileMin.x + m_TileSize, tileMin.y + m_TileSize);

            // Mouse interaction for painting
            if (ImGui::IsMouseHoveringRect(tileMin, tileMax) && ImGui::IsMouseDown(0)) {
                TileData previousTile = tile;
                tile.Opacity = m_Opacity;

                if (m_SelectedTextureIndex >= 0) 
                {
                    if (m_FillMode) 
                    {
                        m_TileLayer.FillLayer(m_SelectedTextureIndex, m_ActiveLayer, y, x);
                    }
                    else
                    {
                        tile.UseTexture = true;
                        tile.TextureIndex = m_SelectedTextureIndex;
                    }
                }

                if (m_EraserMode) 
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
    float offset = m_TileSize + m_Padding;
    ImVec2 tileMin = ImVec2(cursorPos.x + x * offset, cursorPos.y + y * offset);
    ImVec2 tileMax = ImVec2(tileMin.x + m_TileSize, tileMin.y + m_TileSize);

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