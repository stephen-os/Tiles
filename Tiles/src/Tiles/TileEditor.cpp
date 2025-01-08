#include "TileEditor.h"

#include <string>
#include <iostream>
#include <queue>

#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>

#include "TileSerializer.h"

TileEditor::TileEditor()
    : m_NumLayers(0), m_ActiveLayer(0), 
    m_Width(0), m_Height(0), m_Padding(0.0), m_TileSize(0.0),
    m_EraserMode(false), m_FillMode(false), m_Opacity(1.0f), 
    m_SelectedTextureIndex(-1), m_CurrentScenePath("res/maps/tiles.json"),
    m_TileAtlasPath("res/texture/world_tileset.png")
{
    InitEditor(20, 20);
}

TileEditor::~TileEditor()
{
}

void TileEditor::InitEditor(int width, int height)
{
    m_NumLayers = 1; 

    m_Width = width;
    m_Height = height;
    m_Padding = 0.0f;
    m_TileSize = 40.0f;

    m_TileLayers = TileSerializer::Deserialize(m_CurrentScenePath);

    if (m_TileLayers.size() == 0)
    {
        AddLayer();
    }
    else
    {
        m_NumLayers = (int)m_TileLayers.size();
    }

    LoadTextures();

    UpdateMatrices();
}

void TileEditor::Shutdown()
{
    // TileSerializer::Serialize(m_TileLayers, m_CurrentScenePath);
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
        UpdateMatrices();
    }

    ImGui::SameLine();

    if (ImGui::Button("Save Scene")) {
        TileSerializer::Serialize(m_TileLayers, m_CurrentScenePath);
    }

    ImGui::Separator();

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
        Undo();
    }

    ImGui::SameLine();

    if (ImGui::Button("Redo")) {
        Redo();
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
    layerNames.reserve(m_TileLayers.size());
    for (const auto& layer : m_TileLayers) {
        layerNames.push_back(layer.m_Name.c_str());
    }
    ImGui::Combo("Active Layer", &m_ActiveLayer, layerNames.data(), static_cast<int>(layerNames.size()));

    // Add Layer Button
    if (ImGui::Button("Add Layer"))
        AddLayer("Layer " + std::to_string(m_NumLayers + 1));

    ImGui::SameLine();

    // Delete Layer Button
    if (ImGui::Button("Delete Layer"))
        DeleteLayer();

    ImGui::SameLine();

    // Clear Layer Button
    if (ImGui::Button("Clear Layer"))
        ClearLayer();

    ImGui::Separator();

    // Editable layer name field
    TileLayer& activeLayer = m_TileLayers[m_ActiveLayer];
    char layerNameBuffer[128];
    strncpy_s(layerNameBuffer, activeLayer.m_Name.c_str(), sizeof(layerNameBuffer) - 1);
    layerNameBuffer[sizeof(layerNameBuffer) - 1] = '\0';

    // ImGui input text for editable layer name
    if (ImGui::InputText("Layer Name", layerNameBuffer, sizeof(layerNameBuffer)))
    {
        activeLayer.m_Name = std::string(layerNameBuffer);
    }

    ImGui::Separator();

    // Render tiles
    float childWidth = m_Width * (m_TileSize + m_Padding) + 15;
    float childHeight = m_Height * (m_TileSize + m_Padding) + 15;
    ImGui::BeginChild("RenderTiles", ImVec2(childWidth, childHeight), true, 0);
    
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    for (int y = 0; y < m_Height; y++)
    {
        for (int x = 0; x < m_Width; x++)
        {
            ImGui::PushID(y * m_Width + x);

            Tile& tile = GetTile(x, y);
            float offset = m_TileSize + m_Padding;
            ImVec2 tileMin = ImVec2(cursorPos.x + x * offset, cursorPos.y + y * offset);
            ImVec2 tileMax = ImVec2(tileMin.x + m_TileSize, tileMin.y + m_TileSize);

            // Mouse interaction for painting
            if (ImGui::IsMouseHoveringRect(tileMin, tileMax) && ImGui::IsMouseDown(0)) {
                Tile previousTile = tile;
                tile.m_Opacity = m_Opacity;

                if (m_SelectedTextureIndex >= 0) {
                    if (m_FillMode) {
                        FillLayer(x, y);
                    }
                    else {
                        tile.m_UseTexture = true;
                        tile.m_TextureIndex = m_SelectedTextureIndex;
                    }
                }

                if (m_EraserMode) {
                    ResetTile(x, y);
                }

                RecordAction(x, y, previousTile, tile);
                UpdateMatrices();
            }

            // Draw the tile
            if (tile.m_UseTexture && tile.m_TextureIndex >= 0)
            {
                intptr_t textureID = (intptr_t)m_Atlas.GetTextureID();

                glm::vec4 texCoords = m_Atlas.GetTexCoords(tile.m_TextureIndex);
                ImVec2 xy = ImVec2(texCoords.x, texCoords.y);
                ImVec2 zw = ImVec2(texCoords.z, texCoords.w);

                const int r = static_cast<int>(tile.m_Opacity * 255);
                const int g = static_cast<int>(tile.m_Opacity * 255);
                const int b = static_cast<int>(tile.m_Opacity * 255);
                const int a = 255;

                const ImU32 color = IM_COL32(r, g, b, a); 

                ImGui::GetWindowDrawList()->AddImage((void*)textureID, tileMin, tileMax, xy, zw, color);
            }
            else
            {
                const float r = tile.m_Color.r;
                const float g = tile.m_Color.g;
                const float b = tile.m_Color.b;
                const float a = tile.m_Color.a;

				const ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));

                ImGui::GetWindowDrawList()->AddRectFilled(tileMin, tileMax, color);
            }

            ImGui::GetWindowDrawList()->AddRect(tileMin, tileMax, IM_COL32(169, 169, 169, 255));
            ImGui::PopID();
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

// Add a new layer
void TileEditor::AddLayer(std::string name)
{
    TileLayer layer;
    layer.m_Name = name; 
    layer.m_Tiles.resize(m_Height, std::vector<Tile>(m_Width));
    for (int y = 0; y < m_Height; ++y)
    {
        for (int x = 0; x < m_Width; ++x)
        {
            Tile& tile = layer.m_Tiles[y][x];
            tile.m_Color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
            tile.m_UseTexture = false;
            tile.m_Opacity = 1.0f;
            tile.m_TextureIndex = -1; 
        }
    }
    m_TileLayers.push_back(layer);
    m_NumLayers = static_cast<int>(m_TileLayers.size());
    m_ActiveLayer = m_NumLayers - 1;
}

void TileEditor::DeleteLayer()
{
    if (m_NumLayers > 1)
    {
        // Delete the active layer
        m_TileLayers.erase(m_TileLayers.begin() + m_ActiveLayer);
        m_NumLayers = static_cast<int>(m_TileLayers.size());

        // Adjust active layer index
        if (m_ActiveLayer >= m_NumLayers)
        {
            m_ActiveLayer = m_NumLayers - 1;
        }
    }

    UpdateMatrices();
}

void TileEditor::ClearLayer()
{
    for (int y = 0; y < m_Height; y++)
    {
        for (int x = 0; x < m_Width; x++)
        {
			ResetTile(x, y);
        }
    }

    UpdateMatrices();
}

Tile& TileEditor::GetTile(int x, int y)
{
    return m_TileLayers[m_ActiveLayer].m_Tiles[y][x];
}

void TileEditor::ResetTile(int x, int y)
{
    Tile& tile = m_TileLayers[m_ActiveLayer].m_Tiles[y][x];
    tile.m_Color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    tile.m_UseTexture = false;
    tile.m_Opacity = 1.0f;
    tile.m_TextureIndex = -1;
}

void TileEditor::FillLayer(int x, int y)
{
    if (x < 0 || x >= m_Width || y < 0 || y >= m_Height)
        return;

    int originalTextureIndex = m_TileLayers[m_ActiveLayer].m_Tiles[y][x].m_TextureIndex;

    if (originalTextureIndex == m_SelectedTextureIndex)
        return;


    std::queue<std::pair<int, int>> tileQueue;
    tileQueue.push({ x, y });

    const std::vector<std::pair<int, int>> directions = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };

    while (!tileQueue.empty())
    {
        auto [x, y] = tileQueue.front();
        tileQueue.pop();

        if (x < 0 || x >= m_Width || y < 0 || y >= m_Height)
            continue;

        if (m_TileLayers[m_ActiveLayer].m_Tiles[y][x].m_TextureIndex != originalTextureIndex)
           continue;

        m_TileLayers[m_ActiveLayer].m_Tiles[y][x].m_TextureIndex = m_SelectedTextureIndex;
        m_TileLayers[m_ActiveLayer].m_Tiles[y][x].m_UseTexture = true;

        for (const auto& direction : directions)
        {
            int newX = x + direction.first;
            int newY = y + direction.second;
            tileQueue.push({ newX, newY });
        }
    }
}

void TileEditor::UpdateMatrices()
{
    m_Matrices.clear();
    m_Offsets.clear();

    for (int layer = 0; layer < m_NumLayers; ++layer)
    {
        for (int y = 0; y < m_Height; ++y)
        {
            for (int x = 0; x < m_Width; ++x)
            {
                Tile& tile = m_TileLayers[layer].m_Tiles[y][x];
                if (tile.m_UseTexture)
                {
                    glm::mat4 translation = glm::translate(glm::mat4(1.0f),  glm::vec3(x, y, layer * 0.01f));
                    m_Matrices.push_back(translation);
                    m_Offsets.push_back(m_Atlas.GetOffset(tile.m_TextureIndex));
                }
            }
        }
    }
}

void TileEditor::RecordAction(int x, int y, const Tile& previous, const Tile& current) {
    // Check if the stack is not empty and the top action matches the new action
    if (!m_UndoStack.empty()) {
        const TileAction& topAction = m_UndoStack.top();
        if (topAction.x == x && topAction.y == y &&
            topAction.newState.m_TextureIndex == current.m_TextureIndex &&
            topAction.newState.m_UseTexture == current.m_UseTexture &&
            topAction.newState.m_Opacity == current.m_Opacity) {
            // If the action is redundant, do not push it to the stack
            return;
        }
    }

    // Push the new action if it represents a meaningful change
    m_UndoStack.push({ x, y, previous, current });

    // Clear redo stack since this is a new action
    while (!m_RedoStack.empty()) {
        m_RedoStack.pop();
    }
}

void TileEditor::Undo() {
    if (m_UndoStack.empty()) {
        std::cerr << "No actions to undo!" << std::endl;
        return;
    }

    TileAction lastAction = m_UndoStack.top();
    m_UndoStack.pop();

    Tile& tile = GetTile(lastAction.x, lastAction.y);
    tile = lastAction.previousState;  // Restore the previous state
    UpdateMatrices();

    m_RedoStack.push(lastAction);  // Optional: Push to redo stack
}

void TileEditor::Redo() {
    if (m_RedoStack.empty()) {
        std::cerr << "No actions to redo!" << std::endl;
        return;
    }

    TileAction lastAction = m_RedoStack.top();
    m_RedoStack.pop();

    Tile& tile = GetTile(lastAction.x, lastAction.y);
    tile = lastAction.newState;  // Restore the new state
    UpdateMatrices();

    m_UndoStack.push(lastAction);  // Push back to undo stack
}