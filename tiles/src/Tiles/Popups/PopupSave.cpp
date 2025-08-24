#include "PopupSave.h"

#include "Core/Constants.h"

namespace Tiles
{
    PopupSave::PopupSave()
    {
    }

    void PopupSave::Show(Ref<Context> context)
    {
        if (!context || !context->HasProject())
            return;

        m_Context = context;
        m_IsVisible = true;
        m_ShowMessage = false;
        m_MessageTimer = 0.0f;
    }

    void PopupSave::Render()
    {
        if (!m_IsVisible)
            return;

        // Update message timer
        if (m_ShowMessage)
        {
            m_MessageTimer += ImGui::GetIO().DeltaTime;
            if (m_MessageTimer >= MESSAGE_DISPLAY_TIME)
            {
                m_ShowMessage = false;
                m_MessageTimer = 0.0f;
            }
        }

        RenderSaveDialog();
    }

    void PopupSave::RenderSaveDialog()
    {
        ImGui::SetNextWindowSize(ImVec2(350, 150), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Save Project", &m_IsVisible,
            ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize))
        {
            if (!m_Context || !m_Context->HasProject())
            {
                ImGui::Text("No project to save!");
                ImGui::End();
                return;
            }

            auto project = m_Context->GetProject();

            ImGui::Text("Save current project:");
            ImGui::Text("Project: %s", project->GetProjectName().c_str());

            if (project->IsNew())
            {
                ImGui::TextColored(UI::Color::TextError, "New project - will prompt for location");
            }
            else
            {
                ImGui::Text("Location: %s", project->GetFilePath().string().c_str());
            }

            if (project->HasUnsavedChanges())
            {
                ImGui::TextColored(UI::Color::TextError, "Project has unsaved changes");
            }
            else
            {
                ImGui::TextColored(UI::Color::Green, "Project is up to date");
            }

            // Show message if present
            if (m_ShowMessage)
            {
                ImGui::Spacing();
                ImGui::TextColored(UI::Color::TextHint, "%s", m_SaveMessage.c_str());
            }

            ImGui::Spacing();
            ImGui::Separator();

            RenderActionButtons();
        }
        ImGui::End();
    }

    void PopupSave::RenderActionButtons()
    {
        float buttonWidth = 80.0f;
        float spacing = ImGui::GetContentRegionAvail().x - (buttonWidth * 2 + UI::Component::SpaceBetween);

        if (ImGui::Button("Save", ImVec2(buttonWidth, 0)))
        {
            ExecuteSave();
        }

        ImGui::SameLine(0, spacing);

        if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
        {
            Close();
        }
    }

    void PopupSave::ExecuteSave()
    {
        if (!m_Context || !m_Context->HasProject())
        {
            m_SaveMessage = "No project to save!";
            m_ShowMessage = true;
            m_MessageTimer = 0.0f;
            return;
        }

        try
        {
            bool success = m_Context->SaveProject();

            if (success)
            {
                m_SaveMessage = "Project saved successfully!";
                m_ShowMessage = true;
                m_MessageTimer = 0.0f;

                // Close after a brief delay to show success message
                ImGui::SetWindowFocus(nullptr);
            }
            else
            {
                m_SaveMessage = "Failed to save project. Check file permissions.";
                m_ShowMessage = true;
                m_MessageTimer = 0.0f;
            }
        }
        catch (const std::exception& e)
        {
            m_SaveMessage = std::string("Save error: ") + e.what();
            m_ShowMessage = true;
            m_MessageTimer = 0.0f;
        }
    }
}