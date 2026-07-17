#include "PopupError.h"

#include "../UI/Widgets.h"
#include "../UI/Theme.h"

#include "imgui.h"

namespace Tiles::Editor
{
    PopupError::PopupError(EditorHost& host) : Popup(host) {}

    void PopupError::OnRender()
    {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Error", &m_IsVisible, ImGuiWindowFlags_Modal | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextColored(UI::GetTheme().Danger, "%s", m_Message.c_str());
            ImGui::Separator();
            if (UI::Button("OK", UI::ButtonVariant::Default, ImVec2(80.0f, 0.0f)))
            {
                Hide();
            }
        }
        ImGui::End();
    }
}
