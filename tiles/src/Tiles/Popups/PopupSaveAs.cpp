#include "PopupSaveAs.h"

#include "ImGuiFileDialog.h"
#include <filesystem>

namespace Tiles
{
    PopupSaveAs::PopupSaveAs()
    {
        strcpy_s(m_FileName, sizeof(m_FileName), "Untitled");
        m_Directory = ".";
    }

    void PopupSaveAs::Show(Ref<Context> context)
    {
        if (!context || !context->HasProject())
            return;

        m_Context = context;
        m_IsVisible = true;
        m_ShowMessage = false;
        m_MessageTimer = 0.0f;

        // Initialize with current project name and directory
        auto project = m_Context->GetProject();
        std::string projectName = project->GetProjectName();

        // Remove any existing extension from project name
        size_t extensionPos = projectName.find_last_of('.');
        if (extensionPos != std::string::npos)
        {
            projectName = projectName.substr(0, extensionPos);
        }

        strncpy_s(m_FileName, sizeof(m_FileName), projectName.c_str(), _TRUNCATE);

        // Set directory based on current file path or use current directory
        if (!project->IsNew())
        {
            std::filesystem::path currentPath = project->GetFilePath();
            m_Directory = currentPath.parent_path().string();
        }
        else
        {
            m_Directory = ".";
        }

        ValidateFileName();
    }

    void PopupSaveAs::Render()
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

        RenderSaveAsDialog();

        if (m_ShowDirectorySelector)
        {
            ShowDirectoryDialog();
        }
    }

    void PopupSaveAs::RenderSaveAsDialog()
    {
        ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Save Project As", &m_IsVisible,
            ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize))
        {
            if (!m_Context || !m_Context->HasProject())
            {
                ImGui::Text("No project to save!");
                ImGui::End();
                return;
            }

            ImGui::Text("Save project as new file:");
            ImGui::Separator();

            RenderFileSettings();

            // Show full path preview
            ImGui::Spacing();
            ImGui::Text("Full path:");
            ImGui::TextColored(UI::Color::TextHint, "%s", GetFullFilePath().c_str());

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

            RenderActionButtons();
        }
        ImGui::End();
    }

    void PopupSaveAs::RenderFileSettings()
    {
        // Directory input
        ImGui::Text("Directory:");
        ImGui::SetNextItemWidth(-80);
        ImGui::InputText("##Directory", m_Directory.data(), m_Directory.capacity(), ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("Browse..."))
        {
            IGFD::FileDialogConfig config;
            config.path = m_Directory.empty() ? "." : m_Directory.c_str();
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
        if (ImGui::InputText("##FileName", m_FileName, sizeof(m_FileName)))
        {
            ValidateFileName();
        }
        ImGui::SameLine();
        ImGui::Text("%s", DEFAULT_EXTENSION);

        if (!m_FileNameValid)
        {
            ImGui::TextColored(UI::Color::TextError, "Invalid file name!");
        }
    }

    void PopupSaveAs::RenderActionButtons()
    {
        float buttonWidth = 80.0f;
        float spacing = ImGui::GetContentRegionAvail().x - (buttonWidth * 2 + UI::Component::SpaceBetween);

        bool canSave = m_FileNameValid && strlen(m_FileName) > 0;

        if (ImGui::Button("Save", ImVec2(buttonWidth, 0)) && canSave)
        {
            ExecuteSaveAs();
        }

        if (!canSave)
        {
            ImGui::SetItemTooltip("Enter a valid file name to save");
        }

        ImGui::SameLine(0, spacing);

        if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
        {
            Close();
        }
    }

    void PopupSaveAs::ExecuteSaveAs()
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
            std::string fullPath = GetFullFilePath();
            std::filesystem::path filePath(fullPath);

            // Create directory if it doesn't exist
            std::filesystem::path parentDir = filePath.parent_path();
            if (!parentDir.empty() && !std::filesystem::exists(parentDir))
            {
                std::filesystem::create_directories(parentDir);
            }

            // Check if file already exists
            if (std::filesystem::exists(filePath))
            {
                m_SaveMessage = "File already exists! Choose a different name or confirm overwrite.";
                m_ShowMessage = true;
                m_MessageTimer = 0.0f;
                return;
            }

            bool success = m_Context->SaveProjectAs(filePath);

            if (success)
            {
                m_SaveMessage = "Project saved successfully!";
                m_ShowMessage = true;
                m_MessageTimer = 0.0f;

                // Close after showing success message briefly
                ImGui::SetWindowFocus(nullptr);
            }
            else
            {
                m_SaveMessage = "Failed to save project. Check file permissions and path.";
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

        // Add extension if not present
        if (fileName.find(DEFAULT_EXTENSION) == std::string::npos)
        {
            fileName += DEFAULT_EXTENSION;
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