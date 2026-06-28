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
            m_ProjectSavedSuccessfully = false;
            InitializeFromCurrentProject();
            ValidateFileName();
            m_FirstShow = false;
        }

        ImGui::SetNextWindowSizeConstraints(ImVec2(600, 0), ImVec2(600, FLT_MAX));
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Save Project As", &m_IsVisible, ImGuiWindowFlags_Modal | ImGuiWindowFlags_AlwaysAutoResize))
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

            ImGui::Spacing();
            ImGui::Text("Full path:");
            std::string fullPath = GetFullFilePath();
            ImGui::TextColored(UI::Color::TextHint, "%s", fullPath.c_str());

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
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                    ImGui::Text("%s", m_SaveMessage.c_str());
                    ImGui::PopStyleColor();
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
        if (!m_Context || !m_Context->HasProject())
            return;

        auto project = m_Context->GetProject();
        std::string projectName = project->GetProjectName();

        size_t extensionPos = projectName.find_last_of('.');
        if (extensionPos != std::string::npos)
        {
            projectName = projectName.substr(0, extensionPos);
        }

        m_FileName = projectName;

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

        ImGui::Text("File name:");
        ImGui::SetNextItemWidth(450.0f);
        if (ImGui::InputText("##FileName", m_FileName.data(), m_FileName.capacity() + 1))
        {
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

    void PopupSaveAs::RenderActionButtons()
    {
        float buttonWidth = 80.0f;
        float totalWidth = buttonWidth * 2 + ImGui::GetStyle().ItemSpacing.x;
        float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);

        bool canSave = m_FileNameValid && !m_FileName.empty();

        if (ImGui::Button("Save", ImVec2(buttonWidth, 0)) && canSave)
        {
            std::filesystem::path fullPath = GetFullFilePath();
            auto result = m_Context->SaveProjectAs(fullPath);

            m_SaveMessage = result.Message;
            m_ShowMessage = true;
            m_MessageTimer = 0.0f;
            m_ProjectSavedSuccessfully = result.Success;

            if (result.Success)
            {
                ImGui::SetWindowFocus(nullptr);
            }
        }

        if (!canSave)
        {
            ImGui::SetItemTooltip("Enter a valid file name to save");
        }

        ImGui::SameLine();

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

        if (ImGuiFileDialog::Instance()->Display("ChooseSaveDirectoryDlg"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                m_Directory = ImGuiFileDialog::Instance()->GetCurrentPath();
            }
            ImGuiFileDialog::Instance()->Close();
            m_ShowDirectorySelector = false;
        }

        ImGui::PopStyleColor(12);
    }
}