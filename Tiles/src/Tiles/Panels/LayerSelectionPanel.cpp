#include "LayerSelectionPanel.h"

#include "../Core/Layer.h"

#include "../Commands/AddLayerCommand.h"
#include "../Commands/DeleteLayerCommand.h"
#include "../Commands/ClearLayerCommand.h"

#include "imgui.h"

#include "Lumina/Core/Log.h"

namespace Tiles
{
    void LayerSelectionPanel::Render()
    {
        ImGui::Begin("Layer Selection");

        char buffer[256];
        strncpy_s(buffer, m_Layers->GetName().c_str(), sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';

        ImGui::Text("Project Name");
        if (ImGui::InputText("###ProjectName", buffer, sizeof(buffer)))
        {
            m_Layers->SetName(std::string(buffer));
        }

        ImGui::Separator();

		ImGui::Text("Layers");

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

        ImGui::BeginChild("LayerBox", ImVec2(0, 300), true, ImGuiWindowFlags_None);

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

        ImGui::PopStyleColor(3);

        ImGui::EndChild();

        ImGui::PopStyleVar();

        if (ImGui::Button("Move Up"))
        {
            m_Layers->ShiftLayerUp();
        }

        ImGui::SameLine();
        
        if (ImGui::Button("Move Down"))
        {
            m_Layers->ShiftLayerDown();
        }

        ImGui::Separator();

        if (ImGui::Button("Add Layer"))
        {
            m_CommandHistory->ExecuteCommand(MakeUnique<AddLayerCommand>(*m_Layers));
        }

        ImGui::SameLine();

        if (ImGui::Button("Delete Layer"))
        {
			if (m_Layers->GetSize() != 0)
			{
                m_CommandHistory->ExecuteCommand(MakeUnique<DeleteLayerCommand>(*m_Layers));
			}
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear Layer"))
        {
			if (m_Layers->GetSize() != 0)
			{
                m_CommandHistory->ExecuteCommand(MakeUnique<ClearLayerCommand>(*m_Layers));
			}
        }

        ImGui::Separator();

        if (m_Layers->GetSize() != 0)
        {
            Layer& layer = m_Layers->GetLayer(m_Layers->GetActiveLayer());
            char buffer[128];
            strncpy_s(buffer, layer.GetName().c_str(), sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';

            if (ImGui::InputText("Layer Name", buffer, sizeof(buffer)))
            {
                layer.SetName(std::string(buffer));
            }

            ImGui::Separator();
        }

        ImGui::End();
    }
}