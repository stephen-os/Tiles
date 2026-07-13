#pragma once

#include "Popup.h"

#include <string>
#include <filesystem>

namespace Tiles::Editor
{
    // Save-As dialog: pick a directory and file name (project extension appended
    // automatically) and write the project via Session::SaveProjectAs. Shows an
    // inline status message and auto-closes a few seconds after a successful save.
    class PopupSaveAs : public Popup
    {
    public:
        PopupSaveAs(EditorHost& host);
        ~PopupSaveAs() = default;

    protected:
        void OnRender() override;
        void OnUpdate() override;

    private:
        void RenderFileSettings();
        void RenderActionButtons();
        void ValidateFileName();
        void ShowDirectoryDialog();
        std::string GetFullFilePath() const;
        void InitializeFromCurrentProject();

    private:
        // ImGui::InputText writes into this fixed buffer; its size is independent
        // of the string contents so the writable region is always well defined.
        static constexpr size_t FileNameBufferSize = 256;
        char m_FileNameBuffer[FileNameBufferSize] = "Untitled";
        std::filesystem::path m_Directory = ".";
        std::string m_SaveMessage;

        bool m_ShowMessage = false;
        bool m_ShowDirectorySelector = false;
        float m_MessageTimer = 0.0f;
        bool m_FileNameValid = true;
        bool m_FirstShow = true;
        bool m_ProjectSavedSuccessfully = false;

        static constexpr float MESSAGE_DISPLAY_TIME = 3.0f;
    };
}