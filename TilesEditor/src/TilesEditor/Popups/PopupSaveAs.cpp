#include "PopupSaveAs.h"
#include "../UIConstants.h"
#include "../UI/Theme.h"
#include "../UI/Widgets.h"
#include "ImGuiFileDialog.h"
#include "Session/Workspace.h"
#include <filesystem>
#include <algorithm>
#include <cstring>

namespace Tiles::Editor
{
    PopupSaveAs::PopupSaveAs(EditorHost& host) : Popup(host) {}

    void PopupSaveAs::OnRender()
    {
        // Re-seed the dialog from the current project each time it opens; the flag
        // is reset in the "not visible" branch below so this runs once per opening.
        if (m_FirstShow)
        {
            m_ShowMessage = false;
            m_MessageTimer = 0.0f;
            m_ShowDirectorySelector = false;
            m_ProjectSavedSuccessfully = false;
            InitializeFromCurrentProject();
            ValidateFileName();
            m_FirstShow = false;
        }

        ImGui::SetNextWindowSizeConstraints(ImVec2(600, 0), ImVec2(600, FLT_MAX));
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Save Project As", &m_IsVisible, ImGuiWindowFlags_Modal | ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (!Ctx().HasProject())
            {
                ImGui::Text("No project to save!");
                ImGui::End();
                return;
            }

            ImGui::Text("Save project as new file:");
            ImGui::Separator();

            RenderFileSettings();

            ImGui::Spacing();
            ImGui::Text("Full path:");
            std::string fullPath = GetFullFilePath();
            ImGui::TextColored(UI::GetTheme().TextMuted, "%s", fullPath.c_str());

            if (m_ShowMessage)
            {
                ImGui::Spacing();
                if (m_SaveMessage.find("error") != std::string::npos ||
                    m_SaveMessage.find("Failed") != std::string::npos)
                {
                    ImGui::TextColored(UI::GetTheme().Danger, "%s", m_SaveMessage.c_str());
                }
                else
                {
                    ImGui::TextColored(UI::GetTheme().Success, "%s", m_SaveMessage.c_str());
                }
            }

            ImGui::Spacing();
            ImGui::Separator();

            RenderActionButtons();
        }
        ImGui::End();

        if (m_ShowDirectorySelector)
        {
            ShowDirectoryDialog();
        }

        if (!m_IsVisible)
        {
            m_FirstShow = true;
        }

        if (m_ShowMessage && m_ProjectSavedSuccessfully)
        {
            m_MessageTimer += ImGui::GetIO().DeltaTime;
            if (m_MessageTimer >= MESSAGE_DISPLAY_TIME)
            {
                Hide();
            }
        }
    }

    void PopupSaveAs::OnUpdate()
    {
        // A failed save only times out its error message and leaves the popup open
        // to retry; a successful save auto-closes the whole popup (handled in OnRender).
        if (m_ShowMessage && !m_ProjectSavedSuccessfully)
        {
            m_MessageTimer += ImGui::GetIO().DeltaTime;
            if (m_MessageTimer >= MESSAGE_DISPLAY_TIME)
            {
                m_ShowMessage = false;
                m_MessageTimer = 0.0f;
            }
        }
    }

    void PopupSaveAs::InitializeFromCurrentProject()
    {
        if (!Ctx().HasProject())
            return;

        auto project = Ctx().GetProject();
        std::string projectName = project->GetProjectName();

        size_t extensionPos = projectName.find_last_of('.');
        if (extensionPos != std::string::npos)
        {
            projectName = projectName.substr(0, extensionPos);
        }

        // Safely copy the project name into the fixed input buffer.
        size_t copyLen = std::min(projectName.size(), FileNameBufferSize - 1);
        std::memcpy(m_FileNameBuffer, projectName.c_str(), copyLen);
        m_FileNameBuffer[copyLen] = '\0';

        if (!project->IsNew())
        {
            m_Directory = project->GetFilePath().parent_path();
        }
        else
        {
            m_Directory = ".";
        }
    }

    void PopupSaveAs::RenderFileSettings()
    {
        ImGui::Text("Directory:");
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", m_Directory.string().c_str());
        ImGui::SameLine();
        if (UI::Button("Browse..."))
        {
            IGFD::FileDialogConfig config;
            config.path = m_Directory.empty() ? "." : m_Directory.string().c_str();
            config.flags = ImGuiFileDialogFlags_Modal;
            config.countSelectionMax = 1;

            ImGuiFileDialog::Instance()->OpenDialog(
                "ChooseSaveDirectoryDlg",
                "Choose Save Directory",
                nullptr,
                config
            );

            m_ShowDirectorySelector = true;
        }

        ImGui::Spacing();

        ImGui::Text("File name:");
        ImGui::SetNextItemWidth(450.0f);
        if (ImGui::InputText("##FileName", m_FileNameBuffer, FileNameBufferSize))
        {
            ValidateFileName();
        }
        ImGui::SameLine();
        ImGui::Text("%s", File::ProjectExtension);

        if (!m_FileNameValid)
        {
            ImGui::TextColored(UI::GetTheme().Danger, "Invalid file name!");
        }
    }

    void PopupSaveAs::RenderActionButtons()
    {
        float buttonWidth = 80.0f;
        float totalWidth = buttonWidth * 2 + ImGui::GetStyle().ItemSpacing.x;
        float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);

        bool canSave = m_FileNameValid && m_FileNameBuffer[0] != '\0';

        if (UI::Button("Save", UI::ButtonVariant::Primary, ImVec2(buttonWidth, 0)) && canSave)
        {
            std::filesystem::path fullPath = GetFullFilePath();
            auto result = Space().SaveProjectAs(fullPath);

            m_ProjectSavedSuccessfully = result.has_value();
            m_SaveMessage = result ? "Project saved successfully." : result.error().message;
            m_ShowMessage = true;
            m_MessageTimer = 0.0f;

            if (result)
            {
                ImGui::SetWindowFocus(nullptr);
            }
        }

        if (!canSave)
        {
            ImGui::SetItemTooltip("Enter a valid file name to save");
        }

        ImGui::SameLine();

        if (UI::Button("Cancel", UI::ButtonVariant::Default, ImVec2(buttonWidth, 0)))
        {
            Hide();
        }
    }

    void PopupSaveAs::ValidateFileName()
    {
        std::string fileName(m_FileNameBuffer);
        m_FileNameValid = !fileName.empty() &&
            fileName.find_first_of("<>:\"/\\|?*") == std::string::npos &&
            fileName != "." && fileName != "..";
    }

    std::string PopupSaveAs::GetFullFilePath() const
    {
        std::filesystem::path dir(m_Directory);
        std::string fileName(m_FileNameBuffer);

        if (fileName.find(File::ProjectExtension) == std::string::npos)
        {
            fileName += File::ProjectExtension;
        }

        return (dir / fileName).string();
    }

    void PopupSaveAs::ShowDirectoryDialog()
    {
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        // The file dialog inherits the global Tiles::UI theme.
        if (ImGuiFileDialog::Instance()->Display("ChooseSaveDirectoryDlg"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                m_Directory = ImGuiFileDialog::Instance()->GetCurrentPath();
            }
            ImGuiFileDialog::Instance()->Close();
            m_ShowDirectorySelector = false;
        }
    }
}