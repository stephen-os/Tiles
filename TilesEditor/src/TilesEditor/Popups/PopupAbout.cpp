#include "PopupAbout.h"

#include "../UI/Widgets.h"

#include "imgui.h"

namespace Tiles::Editor
{
    PopupAbout::PopupAbout(EditorHost& host) : Popup(host) {}

    void PopupAbout::OnRender()
    {
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("About Tiles", &m_IsVisible, ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize))
        {
            UI::TextTitle("Tiles Editor");
            ImGui::Separator();

            ImGui::Text("Version: 1.0.0");
            ImGui::Text("A tile map editor built with the Tiles engine");

            ImGui::Spacing();
            ImGui::Text("Features:");
            ImGui::BulletText("Multi-layer tile editing");
            ImGui::BulletText("Texture atlas support");
            ImGui::BulletText("Brush, eraser, and fill tools");
            ImGui::BulletText("Undo/redo system");
            ImGui::BulletText("Project save/load");

            ImGui::Spacing();
            ImGui::Separator();

            float buttonWidth = 80.0f;
            float spacing = (ImGui::GetContentRegionAvail().x - buttonWidth) * 0.5f;

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + spacing);
            if (UI::Button("Close", UI::ButtonVariant::Default, ImVec2(buttonWidth, 0)))
            {
                Hide();
            }
        }
        ImGui::End();
    }
}
