#include "LayerSelectionPanel.h"

#include "../Core/Layer.h"

#include "../Commands/AddLayerCommand.h"
#include "../Commands/RemoveLayerCommand.h"
#include "../Commands/ReplaceLayerCommand.h"

#include "imgui.h"

namespace Tiles
{

    void LayerSelectionPanel::OnUIRender()
    {
        ImGui::Begin("Layer Selection", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        strncpy_s(m_ProjectName, m_Layers->GetName().c_str(), sizeof(m_ProjectName) - 1);
        m_ProjectName[sizeof(m_ProjectName) - 1] = '\0';

        ImGui::Text("Project Name");
        if (ImGui::InputText("###ProjectName", m_ProjectName, sizeof(m_ProjectName)))
        {
            m_Layers->SetName(std::string(m_ProjectName));
        }

        if (m_Layers->GetSize() != 0)
        {
            Layer& layer = m_Layers->GetLayer(m_Layers->GetActiveLayer());
            const char* layerName = layer.GetName().c_str();
            char layerNameBuffer[128];
            strncpy_s(layerNameBuffer, layerName, sizeof(layerNameBuffer) - 1);
            layerNameBuffer[sizeof(layerNameBuffer) - 1] = '\0';



            ImGui::Separator();
        }

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::BeginChild("LayerBox", ImVec2(0, 200), true, ImGuiWindowFlags_None);

        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered]);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive]);

        for (uint32_t i = 0; i < m_Layers->GetSize(); i++)
        {
            ImGui::PushID(i);

            Layer& layer = m_Layers->GetLayer(i);

            bool flag = layer.GetVisibility();

            if (ImGui::Checkbox("##Visible", &flag))
            {
                layer.ToggleVisibility();
            }

            ImGui::SameLine();

            if (ImGui::Selectable(layer.GetName().c_str(), i == m_Layers->GetActiveLayer()))
            {
                m_Layers->SetActiveLayer(i);
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
            m_CommandHistory->ExecuteCommand(MakeUnique<AddLayerCommand>(m_Layers->GetSize()));
        }

        ImGui::SameLine();

        if (ImGui::Button("Delete Layer"))
        {
            Layer& layer = m_Layers->GetLayer(m_Layers->GetActiveLayer());
            m_CommandHistory->ExecuteCommand(MakeUnique<RemoveLayerCommand>(m_Layers->GetActiveLayer(), layer));
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear Layer"))
        {
            Layer& oldLayer = m_Layers->GetLayer(m_Layers->GetActiveLayer());
            Layer newLayer = oldLayer;
            newLayer.Clear(); 
            m_CommandHistory->ExecuteCommand(MakeUnique<ReplaceLayerCommand>(m_Layers->GetActiveLayer(), oldLayer, newLayer));
        }

        ImGui::Separator();

        if (m_Layers->GetSize() != 0)
        {
            Layer& layer = m_Layers->GetLayer(m_Layers->GetActiveLayer());
            const char* layerName = layer.GetName().c_str();
            char layerNameBuffer[128];
            strncpy_s(layerNameBuffer, layerName, sizeof(layerNameBuffer) - 1);
            layerNameBuffer[sizeof(layerNameBuffer) - 1] = '\0';

            if (ImGui::InputText("Layer Name", layerNameBuffer, sizeof(layerNameBuffer)))
            {
                layer.SetName(std::string(layerNameBuffer));
            }

            ImGui::Separator();
        }

        ImGui::End();
    }

}