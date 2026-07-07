#include "PopupError.h"

#include "../UIConstants.h"

#include "imgui.h"

namespace Tiles::Editor
{
    PopupError::PopupError(EditorHost& host) : Popup(host) {}

    void PopupError::OnRender()
    {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Error", &m_IsVisible, ImGuiWindowFlags_Modal | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextColored(UI::Color::TextError, "%s", m_Message.c_str());
            ImGui::Separator();
            if (ImGui::Button("OK", ImVec2(UI::Component::ControlButtonWidth, 0)))
            {
                Hide();
            }
        }
        ImGui::End();
    }
}
