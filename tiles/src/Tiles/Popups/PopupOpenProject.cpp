#include "PopupOpenProject.h"

#include "Core/Constants.h"

#include "ImGuiFileDialog.h"
#include <filesystem>

namespace Tiles
{
    PopupOpenProject::PopupOpenProject()
    {
        strcpy_s(m_FileName, sizeof(m_FileName), "");
        m_Directory = ".";
    }

    void PopupOpenProject::Show(Ref<Context> context)
    {
        if (!context)
            return;

        m_Context = context;
        m_IsVisible = true;
        m_ShowMessage = false;
        m_MessageTimer = 0.0f;
        m_FilePathValid = false;

        // Initialize directory to current working directory
        try
        {
            m_Directory = std::filesystem::current_path().string();
        }
        catch (...)
        {
            m_Directory = ".";
        }

        // Clear filename
        strcpy_s(m_FileName, sizeof(m_FileName), "");
    }

    void PopupOpenProject::Render()
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

        RenderOpenDialog();

        if (m_ShowFileSelector)
        {
            ShowFileDialog();
        }
    }

    void PopupOpenProject::RenderOpenDialog()
    {
        ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Open Project", &m_IsVisible,
            ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize))
        {
            ImGui::Text("Open an existing tile map project:");
            ImGui::Separator();

            RenderFileSettings();

            // Show full path preview
            std::string fullPath = GetFullFilePath();
            if (!fullPath.empty() && m_FilePathValid)
            {
                ImGui::Spacing();
                ImGui::Text("Full path:");
                ImGui::TextColored(UI::Color::TextHint, "%s", fullPath.c_str());
            }

            // Show message if present
            if (m_ShowMessage)
            {
                ImGui::Spacing();
                if (m_Message.find("error") != std::string::npos ||
                    m_Message.find("Failed") != std::string::npos ||
                    m_Message.find("not found") != std::string::npos)
                {
                    ImGui::TextColored(UI::Color::TextError, "%s", m_Message.c_str());
                }
                else
                {
                    ImGui::TextColored(UI::Color::Green, "%s", m_Message.c_str());
                }
            }

            ImGui::Spacing();
            ImGui::Separator();

            RenderActionButtons();
        }
        ImGui::End();
    }

    void PopupOpenProject::RenderFileSettings()
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
                "ChooseProjectFileDlg",
                "Choose Project File",
                FILTER_EXTENSION,
                config
            );

            m_ShowFileSelector = true;
        }

        ImGui::Spacing();

        // File name input
        ImGui::Text("Project file:");
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputText("##FileName", m_FileName, sizeof(m_FileName)))
        {
            ValidateFilePath();
        }

        if (strlen(m_FileName) == 0)
        {
            ImGui::TextColored(UI::Color::TextHint, "Select a .tiles project file to open");
        }
        else if (!m_FilePathValid)
        {
            ImGui::TextColored(UI::Color::TextError, "File does not exist or is not a valid .tiles file");
        }
        else
        {
            ImGui::TextColored(UI::Color::Green, "Valid project file");
        }
    }

    void PopupOpenProject::RenderActionButtons()
    {
        float buttonWidth = 80.0f;
        float spacing = ImGui::GetContentRegionAvail().x - (buttonWidth * 2 + UI::Component::SpaceBetween);

        bool canOpen = m_FilePathValid && strlen(m_FileName) > 0;

        if (ImGui::Button("Open", ImVec2(buttonWidth, 0)) && canOpen)
        {
            ExecuteOpen();
        }

        if (!canOpen)
        {
            ImGui::SetItemTooltip("Select a valid project file to open");
        }

        ImGui::SameLine(0, spacing);

        if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
        {
            Close();
        }
    }

    void PopupOpenProject::ExecuteOpen()
    {
        if (!m_Context)
        {
            m_Message = "No context available!";
            m_ShowMessage = true;
            m_MessageTimer = 0.0f;
            return;
        }

        if (!m_FilePathValid || strlen(m_FileName) == 0)
        {
            m_Message = "No valid file selected!";
            m_ShowMessage = true;
            m_MessageTimer = 0.0f;
            return;
        }

        try
        {
            std::string fullPath = GetFullFilePath();
            std::filesystem::path filePath(fullPath);

            if (!std::filesystem::exists(filePath))
            {
                m_Message = "File does not exist!";
                m_ShowMessage = true;
                m_MessageTimer = 0.0f;
                return;
            }

            bool success = m_Context->LoadProject(filePath);

            if (success)
            {
                m_Message = "Project loaded successfully!";
                m_ShowMessage = true;
                m_MessageTimer = 0.0f;

                // Close after showing success message briefly
                ImGui::SetWindowFocus(nullptr);
            }
            else
            {
                m_Message = "Failed to load project. File may be corrupted or invalid.";
                m_ShowMessage = true;
                m_MessageTimer = 0.0f;
            }
        }
        catch (const std::exception& e)
        {
            m_Message = std::string("Load error: ") + e.what();
            m_ShowMessage = true;
            m_MessageTimer = 0.0f;
        }
    }

    void PopupOpenProject::ShowFileDialog()
    {
        if (ImGuiFileDialog::Instance()->Display("ChooseProjectFileDlg"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
                std::filesystem::path path(filePath);

                // Update directory and filename
                m_Directory = path.parent_path().string();
                std::string fileName = path.filename().string();
                strncpy_s(m_FileName, sizeof(m_FileName), fileName.c_str(), _TRUNCATE);

                ValidateFilePath();
            }
            ImGuiFileDialog::Instance()->Close();
            m_ShowFileSelector = false;
        }
    }

    std::string PopupOpenProject::GetFullFilePath() const
    {
        if (strlen(m_FileName) == 0)
            return "";

        std::filesystem::path dir(m_Directory);
        std::string fileName(m_FileName);

        return (dir / fileName).string();
    }

    void PopupOpenProject::ValidateFilePath()
    {
        if (strlen(m_FileName) == 0)
        {
            m_FilePathValid = false;
            return;
        }

        std::string fullPath = GetFullFilePath();
        std::filesystem::path path(fullPath);

        // Check if file exists and has correct extension
        m_FilePathValid = std::filesystem::exists(path) &&
            std::filesystem::is_regular_file(path) &&
            path.extension().string() == FILTER_EXTENSION;
    }
}