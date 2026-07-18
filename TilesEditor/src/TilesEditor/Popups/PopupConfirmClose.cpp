#include "PopupConfirmClose.h"

#include "../UI/Widgets.h"

#include "Session/Session.h"
#include "Session/Workspace.h"
#include "Domain/Project.h"

#include "imgui.h"

namespace Tiles::Editor
{
    PopupConfirmClose::PopupConfirmClose(EditorHost& host) : Popup(host) {}

    void PopupConfirmClose::OnRender()
    {
        ImGui::SetNextWindowSize(ImVec2(380, 0), ImGuiCond_Appearing);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Unsaved Changes", &m_IsVisible, ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
        {
            // The document being closed is the active one (switched to first).
            Session& document = Ctx();

            ImGui::TextWrapped("\"%s\" has unsaved changes. Save before closing?",
                document.GetProject()->GetProjectName().c_str());

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            const ImVec2 buttonSize(110.0f, 0.0f);

            if (UI::Button("Save", UI::ButtonVariant::Primary, buttonSize))
            {
                if (document.GetProject()->IsNew())
                {
                    // Never saved: route to Save As and leave the tab open; the user
                    // can close again once it has a file.
                    Host().OpenPopup(PopupId::SaveAs);
                    Hide();
                }
                else
                {
                    auto result = Space().SaveProject();
                    if (result)
                        Host().CloseActiveConfirmed();
                    else
                        Host().Notify(result.error().message);
                    Hide();
                }
            }

            ImGui::SameLine();

            if (UI::Button("Don't Save", UI::ButtonVariant::Danger, buttonSize))
            {
                Host().CloseActiveConfirmed();
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
