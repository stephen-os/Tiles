#include "TileEditor.h"

#include <string>
#include <iostream>
#include <queue>
#include <filesystem>
#include <iostream>

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include <glm/gtc/matrix_transform.hpp>

#include "TileSerializer.h"
#include "TileObject.h"

#include "Lumina/Renderer/ShaderProgram.h"
#include "Lumina/Renderer/FrameBuffer.h"
#include "Lumina/Renderer/Renderer.h"
#include "Lumina/Utils/FileReader.h"

void TileEditor::Init()
{
    m_ExportPath = "res/outputs/output.png"; 

    m_TileLayer.Create(20, 20);
}

void TileEditor::Shutdown()
{
    return; 
}

void TileEditor::Render()
{
    m_HeaderPanel.Render(); 

    RenderTools();
    RenderLayerSelction();
    
    m_TextureSelectionPanel.Render();
    m_TileViewportPanel.Render(m_TileLayer, m_TextureSelectionPanel.GetTextureAtlas(), m_TextureSelectionPanel.GetSelectedTexture());

    RenderAttributes();
    RenderExport();
}

void TileEditor::RenderTools()
{
    ImGui::Begin("Tools");

    // ImGui::Checkbox("Eraser Mode", &m_Modes.Erase);
 
    // ImGui::Checkbox("Fill Mode", &m_Modes.Fill);

    ImGui::End();
}

void TileEditor::RenderAttributes()
{
    ImGui::Begin("Tile Attributes");

    TileData& tile = m_TileLayer.GetHoveredTile();
    
    if (tile.UseTexture)
        ImGui::Text("Use Texture: Yes");
    else
        ImGui::Text("Use Texture: No");
    
    if (tile.UseTexture && tile.TextureIndex >= 0)
    {
        Lumina::TextureAtlas& atlas = m_TextureSelectionPanel.GetTextureAtlas();
        intptr_t textureID = (intptr_t)atlas.GetTextureID();
        glm::vec4 texCoords = atlas.GetTexCoords(static_cast<int>(tile.TextureIndex));
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

void TileEditor::RenderLayerSelction()
{
    ImGui::Begin("Layer Selection", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::BeginChild("LayerBox", ImVec2(0, 200), true, ImGuiWindowFlags_None);

    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered]);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive]);

    for (uint32_t i = 0; i < m_TileLayer.LayerSize(); i++)
    {

        ImGui::PushID(i);

        bool flag = m_TileLayer.IsLayerVisible(i);

        if (ImGui::Checkbox("##Visible", &flag))
        {
            m_TileLayer.ToggleLayerVisibilty(i);
        }

        ImGui::SameLine();

        if (ImGui::Selectable(m_TileLayer.GetLayerName(i), i == m_TileLayer.GetActiveLayer()))
        {
            m_TileLayer.SetActiveLayer(i);
        }

        ImGui::PopID();
    }

    ImGui::PopStyleColor(2);
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::Separator();

    if (ImGui::Button("Add Layer"))
    {
        m_TileLayer.AddLayer("Layer " + std::to_string(m_TileLayer.LayerSize() + 1));
    }

    ImGui::SameLine();

    if (ImGui::Button("Delete Layer"))
    {
        m_TileLayer.DeleteLayer();
    }

    ImGui::SameLine();

    if (ImGui::Button("Clear Layer"))
        m_TileLayer.ClearLayer();

    ImGui::Separator();

    const char* layerName = m_TileLayer.GetLayerName(m_TileLayer.GetActiveLayer());
    char layerNameBuffer[128];
    strncpy_s(layerNameBuffer, layerName, sizeof(layerNameBuffer) - 1);
    layerNameBuffer[sizeof(layerNameBuffer) - 1] = '\0';

    if (ImGui::InputText("Layer Name", layerNameBuffer, sizeof(layerNameBuffer)))
    {
        m_TileLayer.SetLayerName(std::string(layerNameBuffer));
    }

    ImGui::Separator();

    ImGui::End();
}

void TileEditor::RenderExport()
{
    ImGui::Begin("Export");

    ImGui::Text("Export Path");

    ImGui::PushItemWidth(200.0f);

    char exportPathBuffer[256];
    strncpy_s(exportPathBuffer, m_ExportPath.c_str(), sizeof(exportPathBuffer));
    if (ImGui::InputText("##ExportPath", exportPathBuffer, sizeof(exportPathBuffer)))
    {
        m_ExportPath = exportPathBuffer;
    }

    ImGui::Text("Resolution");

    ImGui::InputInt("##Resolution", &m_TileExporter.GetResolution());
    
    ImGui::PopItemWidth();

    if (ImGui::Button("Export Image"))
    {
		m_TileExporter.Export(m_TileLayer, m_TextureSelectionPanel.GetTextureAtlas(), m_ExportPath);
    }

    ImGui::End();
}