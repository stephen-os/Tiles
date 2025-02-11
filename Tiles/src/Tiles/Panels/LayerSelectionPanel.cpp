#include "LayerSelectionPanel.h"

#include "imgui.h"

// TODO: The pannel should render whether tile layer is created or not. 
void LayerSelectionPanel::Render()
{
    // TODO: This may be a good idea as a failsafe for the other panels
    if (!m_TileLayer) return; // Early return if TileLayer is not set

    ImGui::Begin("Layer Selection", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::BeginChild("LayerBox", ImVec2(0, 200), true, ImGuiWindowFlags_None);

    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered]);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive]);

    for (uint32_t i = 0; i < m_TileLayer->LayerSize(); i++)
    {
        ImGui::PushID(i);

        bool flag = m_TileLayer->IsLayerVisible(i);

        if (ImGui::Checkbox("##Visible", &flag))
        {
            m_TileLayer->ToggleLayerVisibilty(i);
        }

        ImGui::SameLine();

        if (ImGui::Selectable(m_TileLayer->GetLayerName(i), i == m_TileLayer->GetActiveLayer()))
        {
            m_TileLayer->SetActiveLayer(i);
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
        m_TileLayer->AddLayer("Layer " + std::to_string(m_TileLayer->LayerSize() + 1));
    }

    ImGui::SameLine();

    if (ImGui::Button("Delete Layer"))
    {
        m_TileLayer->DeleteLayer();
    }

    ImGui::SameLine();

    if (ImGui::Button("Clear Layer"))
    {
        m_TileLayer->ClearLayer();
    }

    ImGui::Separator();


    if (m_TileLayer->LayerSize() != 0)
    {
        const char* layerName = m_TileLayer->GetLayerName(m_TileLayer->GetActiveLayer());
        char layerNameBuffer[128];
        strncpy_s(layerNameBuffer, layerName, sizeof(layerNameBuffer) - 1);
        layerNameBuffer[sizeof(layerNameBuffer) - 1] = '\0';

        if (ImGui::InputText("Layer Name", layerNameBuffer, sizeof(layerNameBuffer)))
        {
            m_TileLayer->SetLayerName(std::string(layerNameBuffer));
        }
    }

    ImGui::Separator();

    ImGui::End();
}