#include "PopupNewProject.h"

#include "../UIConstants.h"

#include "imgui.h"

namespace Tiles::Editor
{
    PopupNewProject::PopupNewProject(EditorHost& host) : Popup(host) {}

    void PopupNewProject::OnRender()
    {
        ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("New Project", &m_IsVisible, ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize))
        {
            ImGui::Text("Create a new tile map project");
            ImGui::Separator();

            ImGui::Text("Project Name:");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##ProjectName", m_NameBuffer, sizeof(m_NameBuffer));

            ImGui::Spacing();
            ImGui::Separator();

            float buttonWidth = 80.0f;
            float spacing = ImGui::GetContentRegionAvail().x - (buttonWidth * 2 + UI::Component::SpaceBetween);

            if (ImGui::Button("Create", ImVec2(buttonWidth, 0)))
            {
                Host().Doc().CreateProject(std::string(m_NameBuffer));
                Hide();
            }

            ImGui::SameLine(0, spacing);

            if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
            {
                Hide();
            }
        }
        ImGui::End();
    }
}
