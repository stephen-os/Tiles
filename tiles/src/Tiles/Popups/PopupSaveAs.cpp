#include "PopupSaveAs.h"

#include "Core/Constants.h"

#include "ImGuiFileDialog.h"

#include <filesystem>

namespace Tiles
{
    PopupSaveAs::PopupSaveAs(Ref<Context> context) : Popup(context) {}

    void PopupSaveAs::OnRender()
    {
        if (m_FirstShow)
        {
            m_ShowMessage = false;
            m_MessageTimer = 0.0f;
            m_ShowDirectorySelector = false;
            InitializeFromCurrentProject();
            ValidateFileName();
            m_FirstShow = false;
        }

        ImGui::SetNextWindowSize(ImVec2(600, 250), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Save Project As", &m_IsVisible, ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize))
        {
            if (!m_Context || !m_Context->HasProject())
            {
                ImGui::Text("No project to save!");
                ImGui::End();
                return;
            }

            ImGui::Text("Save project as new file:");
            ImGui::Separator();

            RenderBlockFileSettings();

            // Show full path preview
            ImGui::Spacing();
            ImGui::Text("Full path:");
			std::string fullPath = GetFullFilePath();
            ImGui::TextColored(UI::Color::TextHint, "%s", fullPath.c_str());

            // Show message if present
            if (m_ShowMessage)
            {
                ImGui::Spacing();
                if (m_SaveMessage.find("error") != std::string::npos ||
                    m_SaveMessage.find("Failed") != std::string::npos)
                {
                    ImGui::TextColored(UI::Color::TextError, "%s", m_SaveMessage.c_str());
                }
                else
                {
                    ImGui::TextColored(UI::Color::Green, "%s", m_SaveMessage.c_str());
                }
            }

            ImGui::Spacing();
            ImGui::Separator();

            RenderBlockActionButtons();
        }
        ImGui::End();

        // Handle directory selector dialog
        if (m_ShowDirectorySelector)
        {
            ShowDirectoryDialog();
        }

        // Reset first show flag when hidden
        if (!m_IsVisible)
        {
            m_FirstShow = true;
        }
    }

    void PopupSaveAs::OnUpdate()
    {
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
    }

    void PopupSaveAs::InitializeFromCurrentProject()
    {
        if (!m_Context || !m_Context->HasProject())
            return;

        auto project = m_Context->GetProject();
        std::string projectName = project->GetProjectName();

        // Remove any existing extension from project name
        size_t extensionPos = projectName.find_last_of('.');
        if (extensionPos != std::string::npos)
        {
            projectName = projectName.substr(0, extensionPos);
        }

        m_FileName = projectName;

        // Set directory based on current file path or use current directory
        if (!project->IsNew())
        {
            m_Directory = project->GetFilePath().parent_path();
        }
        else
        {
            m_Directory = ".";
        }
    }

    void PopupSaveAs::RenderBlockFileSettings()
    {
        // Directory input
        ImGui::Text("Directory:");
        ImGui::SetNextItemWidth(-80);
        std::string directoryStr = m_Directory.string();
        if (ImGui::InputText("##Directory", directoryStr.data(), directoryStr.capacity() + 1, ImGuiInputTextFlags_ReadOnly))
        {
            // Read-only, so no need to update m_Directory
        }
        ImGui::SameLine();
        if (ImGui::Button("Browse..."))
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

        // File name input
        ImGui::Text("File name:");
        ImGui::SetNextItemWidth(-80);
        if (ImGui::InputText("##FileName", m_FileName.data(), m_FileName.capacity() + 1))
        {
            // Resize string if needed and update
            m_FileName.resize(strlen(m_FileName.data()));
            ValidateFileName();
        }
        ImGui::SameLine();
        ImGui::Text("%s", File::ProjectExtension);

        if (!m_FileNameValid)
        {
            ImGui::TextColored(UI::Color::TextError, "Invalid file name!");
        }
    }

    void PopupSaveAs::RenderBlockActionButtons()
    {
        float buttonWidth = 80.0f;
        float spacing = ImGui::GetContentRegionAvail().x - (buttonWidth * 2 + UI::Component::SpaceBetween);

        bool canSave = m_FileNameValid && !m_FileName.empty();

        if (ImGui::Button("Save", ImVec2(buttonWidth, 0)) && canSave)
        {
            std::filesystem::path fullPath = GetFullFilePath();
            auto result = m_Context->SaveProjectAs(fullPath);

            m_SaveMessage = result.Message;
            m_ShowMessage = true;
            m_MessageTimer = 0.0f;

            if (result.Success)
            {
                ImGui::SetWindowFocus(nullptr);
            }
        }

        if (!canSave)
        {
            ImGui::SetItemTooltip("Enter a valid file name to save");
        }

        ImGui::SameLine(0, spacing);

        if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
        {
            Hide();
        }
    }

    void PopupSaveAs::ValidateFileName()
    {
        std::string fileName(m_FileName);
        m_FileNameValid = !fileName.empty() &&
            fileName.find_first_of("<>:\"/\\|?*") == std::string::npos &&
            fileName != "." && fileName != "..";
    }

    std::string PopupSaveAs::GetFullFilePath() const
    {
        std::filesystem::path dir(m_Directory);
        std::string fileName(m_FileName);

        if (fileName.find(File::ProjectExtension) == std::string::npos)
        {
            fileName += File::ProjectExtension;
        }

        return (dir / fileName).string();
    }

    void PopupSaveAs::ShowDirectoryDialog()
    {
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