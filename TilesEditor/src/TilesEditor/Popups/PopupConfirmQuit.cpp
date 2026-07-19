#include "PopupConfirmQuit.h"

#include "../UI/Widgets.h"

#include "Session/Workspace.h"
#include "Core/Application.h"

#include "imgui.h"

namespace Tiles::Editor
{
    PopupConfirmQuit::PopupConfirmQuit(EditorHost& host) : Popup(host) {}

    void PopupConfirmQuit::OnRender()
    {
        ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_Appearing);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Unsaved Changes###ConfirmQuit", &m_IsVisible, ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
        {
            size_t unsaved = Space().UnsavedDocumentCount();
            ImGui::TextWrapped("%zu document(s) have unsaved changes. Quit without saving?", unsaved);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            const ImVec2 buttonSize(130.0f, 0.0f);

            if (UI::Button("Discard & Quit", UI::ButtonVariant::Danger, buttonSize))
            {
                Application::GetInstance().Shutdown();
                Hide();
            }

            ImGui::SameLine();

            if (UI::Button("Cancel", UI::ButtonVariant::Default, buttonSize))
            {
                Hide();
            }
        }
        ImGui::End();
    }
}
