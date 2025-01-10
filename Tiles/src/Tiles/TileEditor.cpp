#include "TileEditor.h"

#include <string>
#include <iostream>
#include <queue>
#include <filesystem>
#include <iostream>

#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>

#include "TileSerializer.h"

void TileEditor::Init()
{
    m_Atlas.CreateAtlas("res/texture/world_tileset.png", 16, 16);
    m_SavePath = "res/maps/tiles.json";
    m_LoadPath = "res/maps/tiles.json"; 

    m_TileLayer.Init(m_Spec.Width, m_Spec.Height);

    m_ActiveLayer = 0;
    m_SelectedTextureIndex = 0; 

    m_ActiveLayers.resize(m_TileLayer.Size(), true);
    m_TileHovered = { 0.0, 0.0, 0.0 }; 
}

void TileEditor::Shutdown()
{
    return; 
}

void TileEditor::Render()
{
    RenderHeader(); 
    RenderTools();
    RenderLayerSelction();
    RenderTextureSelection(); 
    RenderTiles();
    RenderAttributes(); 
}

void TileEditor::RenderHeader()
{
    ImGui::Begin("Header");

    if (ImGui::Button("Save"))
    {
        m_TileLayer.SaveLayers(m_SavePath);
    }

    ImGui::SameLine();

    {
        ImGui::PushItemWidth(300.0f);
        
        char buffer[256];
        strncpy(buffer, m_SavePath.c_str(), sizeof(buffer));
        if (ImGui::InputText("##FilePath", buffer, sizeof(buffer)))
        {
            m_SavePath = buffer;
        }

        ImGui::PopItemWidth();
    }
    
    ImGui::SameLine();

    if (ImGui::Button("Load"))
    {
        if (std::filesystem::exists(m_LoadPath))
        {
            m_TileLayer.LoadLayers(m_LoadPath);

            m_ActiveLayer = 0;
            m_SelectedTextureIndex = 0;

            m_ActiveLayers.resize(m_TileLayer.Size(), true);
            m_TileHovered = { 0.0, 0.0, 0.0 };
        }
        else
        {
            std::cerr << "Error: Load path does not exist: " << m_LoadPath << std::endl;
        }
    }

    ImGui::SameLine();
    
    {
        ImGui::PushItemWidth(300.0f);

        char buffer[256];
        strncpy(buffer, m_SavePath.c_str(), sizeof(buffer));
        if (ImGui::InputText("##FilePath", buffer, sizeof(buffer)))
        {
            m_LoadPath = buffer;
        }

        ImGui::PopItemWidth();
    }
    
    ImGui::SameLine(); 

    if (ImGui::Button("New"))
    {
        int temp = 20;
        m_TileLayer.Init(temp, temp);
    }

    ImGui::SameLine();

    if (ImGui::Button("Undo"))
        m_TileLayer.UndoAction();

    ImGui::SameLine();

    if (ImGui::Button("Redo"))
        m_TileLayer.RedoAction();

    ImGui::SameLine();

    ImGui::Text("Zoom Level:");

    ImGui::SameLine();

    ImGui::PushItemWidth(300.0f);
    ImGui::SliderFloat("##ZoomSlider", &m_Spec.Zoom, 0.1f, 10.0f, "%.1f");
    ImGui::PopItemWidth();

    ImGui::SameLine();
    
    ImGui::Dummy(ImVec2(10.0f, 0.0f)); 

    ImGui::End();
}

void TileEditor::RenderTools()
{
    ImGui::Begin("Tools");

    ImGui::Checkbox("Eraser Mode", &m_Modes.Erase);
 
    ImGui::Checkbox("Fill Mode", &m_Modes.Fill);

    ImGui::End();
}

void TileEditor::RenderLayerSelction()
{
    ImGui::Begin("Layer Selection", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar);

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
}

void TileEditor::RenderTiles()
{
    ImGui::Begin("Scene");

    for (size_t y = 0; y < m_TileLayer.GetHeight(); y++)
    {
        for (size_t x = 0; x < m_TileLayer.GetWidth(); x++)
        {
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            float offset = m_Spec.TileSize * m_Spec.Zoom;
            ImVec2 tileMin = ImVec2(cursorPos.x + x * offset, cursorPos.y + y * offset);
            ImVec2 tileMax = ImVec2(tileMin.x + offset, tileMin.y + offset);

            const ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

            ImGui::GetWindowDrawList()->AddRectFilled(tileMin, tileMax, color);
            ImGui::GetWindowDrawList()->AddRect(tileMin, tileMax, IM_COL32(169, 169, 169, 255));
        }
    }


    for (size_t layer = 0; layer < m_TileLayer.Size(); layer++)
    {
        for (size_t y = 0; y < m_TileLayer.GetHeight(); y++)
        {
            for (size_t x = 0; x < m_TileLayer.GetWidth(); x++)
            {
                if (!m_ActiveLayers[layer]) continue;

                ImVec2 cursorPos = ImGui::GetCursorScreenPos();       
                TileData& tile = m_TileLayer.GetTileData(layer, y, x);
                float offset = m_Spec.TileSize * m_Spec.Zoom;
                ImVec2 tileMin = ImVec2(cursorPos.x + x * offset, cursorPos.y + y * offset);
                ImVec2 tileMax = ImVec2(tileMin.x + offset, tileMin.y + offset);

                if (ImGui::IsMouseHoveringRect(tileMin, tileMax) && ImGui::IsMouseDown(0)) {
                    TileData previousTile = tile;
                    
                    if (layer == m_ActiveLayer)
                    {
                        TileAction action;

                        action.L = layer;
                        action.X = x;
                        action.Y = y;
                        action.Prev = tile; 

                        if (m_Modes.Fill)
                        {
                            m_TileLayer.FillLayer(m_SelectedTextureIndex, m_ActiveLayer, y, x);
                        }
                        else
                        {
                            tile.UseTexture = true;
                            tile.TextureIndex = m_SelectedTextureIndex;
                        }

                        if (m_Modes.Erase)
                        {
                            m_TileLayer.ClearTile(m_ActiveLayer, y, x);
                        }

                        action.Curr = tile;

                        m_TileLayer.RecordAction(action);

                    }
                }

                if (tile.UseTexture && tile.TextureIndex >= 0)
                {
                    intptr_t textureID = (intptr_t)m_Atlas.GetTextureID();

                    glm::vec4 texCoords = m_Atlas.GetTexCoords(tile.TextureIndex);
                    ImVec2 xy = ImVec2(texCoords.x, texCoords.y);
                    ImVec2 zw = ImVec2(texCoords.z, texCoords.w);

                    const ImU32 color = IM_COL32(255, 255, 255, 255);

                    ImGui::GetWindowDrawList()->AddImage((void*)textureID, tileMin, tileMax, xy, zw, color);
                }

                if (ImGui::IsMouseHoveringRect(tileMin, tileMax))
                {
                    m_TileHovered = { layer, y, x };
                    ImGui::GetWindowDrawList()->AddRect(tileMin, tileMax, IM_COL32(169, 169, 169, 255));
                }

            }
        }
    }
    ImGui::End(); 
}

void TileEditor::RenderTextureSelection()
{
    ImGui::Begin("Texture Selection");

    ImVec2 availableSize = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("TextureSelectionChild", availableSize,true,
        ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);

    for (int y = 0; y < m_Atlas.GetGridHeight(); ++y)
    {
        for (int x = 0; x < m_Atlas.GetGridWidth(); ++x)
        {
            int index = y * m_Atlas.GetGridWidth() + x;

            glm::vec4 texCoords = m_Atlas.GetTexCoords(index);

            ImVec2 buttonSize(m_Spec.TileSize, m_Spec.TileSize);

            ImVec2 xy = ImVec2(texCoords.x, texCoords.y);
            ImVec2 zw = ImVec2(texCoords.z, texCoords.w);

            intptr_t textureID = (intptr_t)m_Atlas.GetTextureID();
            if (ImGui::ImageButton((void*)textureID, buttonSize, xy, zw))
            {
                m_SelectedTextureIndex = index; 
            }

            if (index == m_SelectedTextureIndex)
            {
                ImVec2 min = ImGui::GetItemRectMin();
                ImVec2 max = ImGui::GetItemRectMax();
                ImGui::GetWindowDrawList()->AddRect(min, max, IM_COL32(169, 169, 169, 255), 3.0f, 0, 1.5f);
            }

            if ((index + 1) % m_Atlas.GetGridWidth() != 0)
            {
                ImGui::SameLine();
            }
        }
    }

    // End the scrollable child window
    ImGui::EndChild();

    ImGui::End();
}


void TileEditor::RenderAttributes()
{
    ImGui::Begin("Tile Attributes");

    TileData& tile = m_TileLayer.GetTileData(m_TileHovered.x, m_TileHovered.y, m_TileHovered.z);

    ImGui::Text("Active Layer: %d", m_ActiveLayer);
    ImGui::Text("Tile Position: (%d, %d)", m_TileHovered.y, m_TileHovered.z);
    
    if (tile.UseTexture)
        ImGui::Text("Use Texture: Yes");
    else
        ImGui::Text("Use Texture: No");
    
    if (tile.UseTexture && tile.TextureIndex >= 0)
    {
        intptr_t textureID = (intptr_t)m_Atlas.GetTextureID();
        glm::vec4 texCoords = m_Atlas.GetTexCoords(tile.TextureIndex);
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