#include "PopupOpenProject.h"
#include "../UI/Theme.h"
#include "../UI/Widgets.h"
#include "ImGuiFileDialog.h"
#include "Session/Workspace.h"
#include <filesystem>
#include <cstring>
#include <algorithm>

namespace Tiles::Editor
{
    PopupOpenProject::PopupOpenProject(EditorHost& host) : Popup(host) {}

    void PopupOpenProject::OnRender()
    {
        // Reset dialog state once per opening; the flag is restored in the
        // "not visible" branch below.
        if (m_FirstShow)
        {
            InitializeDialog();
            m_FirstShow = false;
        }

        ImGui::SetNextWindowSizeConstraints(ImVec2(600, 0), ImVec2(600, FLT_MAX));
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Open Project", &m_IsVisible, ImGuiWindowFlags_Modal | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Open an existing tile map project:");
            ImGui::Separator();

            RenderFileSettings();

            auto fullPath = GetFullFilePath();
            if (!fullPath.empty() && m_FilePathValid)
            {
                ImGui::Spacing();
                ImGui::Text("Full path:");
                ImGui::TextColored(UI::GetTheme().TextMuted, "%s", fullPath.string().c_str());
            }

            if (m_ShowMessage)
            {
                ImGui::Spacing();
                if (m_Message.find("error") != std::string::npos ||
                    m_Message.find("Failed") != std::string::npos ||
                    m_Message.find("not found") != std::string::npos)
                {
                    ImGui::TextColored(UI::GetTheme().Danger, "%s", m_Message.c_str());
                }
                else
                {
                    ImGui::TextColored(UI::GetTheme().Success, "%s", m_Message.c_str());
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

        if (!m_IsVisible)
        {
            m_FirstShow = true;
        }

        if (m_ShowMessage && m_ProjectOpenedSuccessfully)
        {
            m_MessageTimer += ImGui::GetIO().DeltaTime;
            if (m_MessageTimer >= MESSAGE_DISPLAY_TIME)
            {
                Hide();
            }
        }
    }

    void PopupOpenProject::OnUpdate()
    {
        // A failed open only times out its error message and leaves the popup open
        // to retry; a successful open auto-closes the popup (handled in OnRender).
        if (m_ShowMessage && !m_ProjectOpenedSuccessfully)
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
        m_ProjectOpenedSuccessfully = false;

        try
        {
            m_Directory = std::filesystem::current_path();
        }
        catch (...)
        {
            m_Directory = ".";
        }

        m_FileNameBuffer[0] = '\0';
    }

    void PopupOpenProject::RenderFileSettings()
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
                "ChooseProjectFileDlg",
                "Choose Project File",
                FILTER_EXTENSION,
                config
            );

            m_ShowFileSelector = true;
        }

        ImGui::Spacing();

        ImGui::Text("Project file:");
        ImGui::SetNextItemWidth(450.0f);
        if (ImGui::InputText("##FileName", m_FileNameBuffer, FileNameBufferSize))
        {
            ValidateFilePath();
        }

        if (m_FileNameBuffer[0] == '\0')
        {
            ImGui::TextColored(UI::GetTheme().TextMuted, "Select a .tiles project file to open");
        }
        else if (!m_FilePathValid)
        {
            ImGui::TextColored(UI::GetTheme().Danger, "File does not exist or is not a valid .tiles file");
        }
        else
        {
            ImGui::TextColored(UI::GetTheme().Success, "Valid project file");
        }
    }

    void PopupOpenProject::RenderActionButtons()
    {
        float buttonWidth = 80.0f;
        float totalWidth = buttonWidth * 2 + ImGui::GetStyle().ItemSpacing.x;
        float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);

        bool canOpen = m_FilePathValid && m_FileNameBuffer[0] != '\0';

        if (UI::Button("Open", UI::ButtonVariant::Primary, ImVec2(buttonWidth, 0)) && canOpen)
        {
            ExecuteOpen();
        }

        if (!canOpen)
        {
            ImGui::SetItemTooltip("Select a valid project file to open");
        }

        ImGui::SameLine();

        if (UI::Button("Cancel", UI::ButtonVariant::Default, ImVec2(buttonWidth, 0)))
        {
            Hide();
        }
    }

    void PopupOpenProject::ExecuteOpen()
    {
        if (!m_FilePathValid || m_FileNameBuffer[0] == '\0')
        {
            m_Message = "No valid file selected!";
            m_ShowMessage = true;
            m_MessageTimer = 0.0f;
            m_ProjectOpenedSuccessfully = false;
            return;
        }

        auto result = Space().LoadProject(GetFullFilePath());

        m_ProjectOpenedSuccessfully = result.has_value();
        m_Message = result ? "Project loaded successfully." : result.error().message;
        m_ShowMessage = true;
        m_MessageTimer = 0.0f;

        if (result)
        {
            ImGui::SetWindowFocus(nullptr);
        }
    }

    void PopupOpenProject::ShowFileDialog()
    {
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        // The file dialog inherits the global Tiles::UI theme.
        if (ImGuiFileDialog::Instance()->Display("ChooseProjectFileDlg"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
                std::filesystem::path path(filePath);

                m_Directory = path.parent_path();

                // Safely copy filename to fixed buffer
                std::string filename = path.filename().string();
                size_t copyLen = std::min(filename.size(), FileNameBufferSize - 1);
                std::memcpy(m_FileNameBuffer, filename.c_str(), copyLen);
                m_FileNameBuffer[copyLen] = '\0';

                ValidateFilePath();
            }
            ImGuiFileDialog::Instance()->Close();
            m_ShowFileSelector = false;
        }
    }

    std::filesystem::path PopupOpenProject::GetFullFilePath() const
    {
        if (m_FileNameBuffer[0] == '\0')
            return {};

        return m_Directory / m_FileNameBuffer;
    }

    void PopupOpenProject::ValidateFilePath()
    {
        if (m_FileNameBuffer[0] == '\0')
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