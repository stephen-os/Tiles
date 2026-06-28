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
                ImGui::TextColored(UI::Color::TextHint, "%s", fullPath.string().c_str());
            }

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
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                    ImGui::Text("%s", m_Message.c_str());
                    ImGui::PopStyleColor();
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

        ImGui::Text("Project file:");
        ImGui::SetNextItemWidth(450.0f);
        if (ImGui::InputText("##FileName", m_FileName.data(), m_FileName.capacity() + 1))
        {
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
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            ImGui::Text("Valid project file");
            ImGui::PopStyleColor();
        }
    }

    void PopupOpenProject::RenderActionButtons()
    {
        float buttonWidth = 80.0f;
        float totalWidth = buttonWidth * 2 + ImGui::GetStyle().ItemSpacing.x;
        float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);

        bool canOpen = m_FilePathValid && !m_FileName.empty();

        if (ImGui::Button("Open", ImVec2(buttonWidth, 0)) && canOpen)
        {
            ExecuteOpen();
        }

        if (!canOpen)
        {
            ImGui::SetItemTooltip("Select a valid project file to open");
        }

        ImGui::SameLine();

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
            m_ProjectOpenedSuccessfully = false;
            return;
        }

        if (!m_FilePathValid || m_FileName.empty())
        {
            m_Message = "No valid file selected!";
            m_ShowMessage = true;
            m_MessageTimer = 0.0f;
            m_ProjectOpenedSuccessfully = false;
            return;
        }

        auto result = m_Context->LoadProject(GetFullFilePath());

        m_Message = result.Message;
        m_ShowMessage = true;
        m_MessageTimer = 0.0f;
        m_ProjectOpenedSuccessfully = result.Success;

        if (result.Success)
        {
            ImGui::SetWindowFocus(nullptr);
        }
    }

    void PopupOpenProject::ShowFileDialog()
    {
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.95f));
        ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.45f, 0.45f, 0.45f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));

        if (ImGuiFileDialog::Instance()->Display("ChooseProjectFileDlg"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
                std::filesystem::path path(filePath);

                m_Directory = path.parent_path();
                m_FileName = path.filename().string();

                ValidateFilePath();
            }
            ImGuiFileDialog::Instance()->Close();
            m_ShowFileSelector = false;
        }

        ImGui::PopStyleColor(12);
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