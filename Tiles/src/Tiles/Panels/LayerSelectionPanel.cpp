#include "LayerSelectionPanel.h"

#include "../Core/Layer.h"

#include "imgui.h"

namespace Tiles
{

    // TODO: The pannel should render whether tile layer is created or not. 
    void LayerSelectionPanel::Render()
    {
        // TODO: This may be a good idea as a failsafe for the other panels
        if (!m_Layers) return; // Early return if TileLayer is not set

        ImGui::Begin("Layer Selection", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar);

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
            m_State->PushLayer(m_Layers->GetActiveLayer(), Layer(), StateType::Layer_Insert);
            m_Layers->NewLayer();
        }

        ImGui::SameLine();

        if (ImGui::Button("Delete Layer"))
        {
            m_State->PushLayer(m_Layers->GetActiveLayer(), Layer(), StateType::Layer_Delete);
            m_Layers->DeleteLayer();
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear Layer"))
        {
            m_State->PushLayer(m_Layers->GetActiveLayer(), Layer(), StateType::Layer_Replace);
            m_Layers->ClearLayer();
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
        }

        ImGui::Separator();

        ImGui::End();
    }

}