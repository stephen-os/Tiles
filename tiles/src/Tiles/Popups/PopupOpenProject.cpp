#include "PopupOpenProject.h"
#include "Core/Constants.h"
#include "ImGuiFileDialog.h"
#include <filesystem>

namespace Tiles
{
    PopupOpenProject::PopupOpenProject(Ref<Context> context) : Popup(context) {}

    void PopupOpenProject::OnRender()
    {
        if (m_FirstShow)
        {
            InitializeDialog();
            m_FirstShow = false;
        }

        ImGui::SetNextWindowSizeConstraints(ImVec2(500, 0), ImVec2(500, FLT_MAX));
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Open Project", &m_IsVisible, ImGuiWindowFlags_Modal | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Open an existing tile map project:");
            ImGui::Separator();

            RenderFileSettings();

            // Show full path preview
            auto fullPath = GetFullFilePath();
            if (!fullPath.empty() && m_FilePathValid)
            {
                ImGui::Spacing();
                ImGui::Text("Full path:");
                ImGui::TextColored(UI::Color::TextHint, "%s", fullPath.string().c_str());
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

        if (m_ShowFileSelector)
        {
            ShowFileDialog();
        }

        // Reset first show flag when hidden
        if (!m_IsVisible)
        {
            m_FirstShow = true;
        }
    }

    void PopupOpenProject::OnUpdate()
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

    void PopupOpenProject::InitializeDialog()
    {
        m_ShowMessage = false;
        m_MessageTimer = 0.0f;
        m_FilePathValid = false;
        m_ShowFileSelector = false;

        // Initialize directory to current working directory
        try
        {
            m_Directory = std::filesystem::current_path();
        }
        catch (...)
        {
            m_Directory = ".";
        }

        // Clear filename
        m_FileName.clear();
    }

    void PopupOpenProject::RenderFileSettings()
    {
        ImGui::Text("Directory:");
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", m_Directory.string().c_str());
        ImGui::SameLine();
        if (ImGui::Button("Browse..."))
        {
            IGFD::FileDialogConfig config;
            config.path = m_Directory.empty() ? "." : m_Directory.string().c_str();
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
        if (ImGui::InputText("##FileName", m_FileName.data(), m_FileName.capacity() + 1))
        {
            // Resize string if needed and update
            m_FileName.resize(strlen(m_FileName.data()));
            ValidateFilePath();
        }

        if (m_FileName.empty())
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

        bool canOpen = m_FilePathValid && !m_FileName.empty();

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
            Hide();
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

        if (!m_FilePathValid || m_FileName.empty())
        {
            m_Message = "No valid file selected!";
            m_ShowMessage = true;
            m_MessageTimer = 0.0f;
            return;
        }

        auto result = m_Context->LoadProject(GetFullFilePath());

        m_Message = result.Message;
        m_ShowMessage = true;
        m_MessageTimer = 0.0f;

        if (result.Success)
        {
            // Close after showing success message briefly
            ImGui::SetWindowFocus(nullptr);
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
                m_Directory = path.parent_path();
                m_FileName = path.filename().string();

                ValidateFilePath();
            }
            ImGuiFileDialog::Instance()->Close();
            m_ShowFileSelector = false;
        }
    }

    std::filesystem::path PopupOpenProject::GetFullFilePath() const
    {
        if (m_FileName.empty())
            return {};

        return m_Directory / m_FileName;
    }

    void PopupOpenProject::ValidateFilePath()
    {
        if (m_FileName.empty())
        {
            m_FilePathValid = false;
            return;
        }

        auto fullPath = GetFullFilePath();

        m_FilePathValid = std::filesystem::exists(fullPath) &&
            std::filesystem::is_regular_file(fullPath) &&
            fullPath.extension().string() == FILTER_EXTENSION;
    }
}