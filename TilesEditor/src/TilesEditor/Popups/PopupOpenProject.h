#pragma once
#include "Popup.h"
#include <string>
#include <filesystem>

namespace Tiles
{
    /// Open-Project dialog: browse to or type a .tiles file, validate that it
    /// exists and has the right extension, then load it via Context::LoadProject.
    /// Shows an inline status message and auto-closes shortly after a successful open.
    class PopupOpenProject : public Popup
    {
    public:
        PopupOpenProject(std::shared_ptr<Context> context);
        ~PopupOpenProject() = default;

    protected:
        void OnRender() override;
        void OnUpdate() override;

    private:
        void RenderFileSettings();
        void RenderActionButtons();
        void ExecuteOpen();
        void ValidateFilePath();
        void ShowFileDialog();
        std::filesystem::path GetFullFilePath() const;
        void InitializeDialog();

    private:
        // ImGui::InputText writes into this fixed buffer; its size is independent
        // of the string contents so the writable region is always well defined.
        static constexpr size_t FileNameBufferSize = 256;
        char m_FileNameBuffer[FileNameBufferSize] = {};
        std::filesystem::path m_Directory = ".";
        std::string m_Message;
        bool m_ShowMessage = false;
        bool m_ShowFileSelector = false;
        float m_MessageTimer = 0.0f;
        bool m_FilePathValid = false;
        bool m_FirstShow = true;
        bool m_ProjectOpenedSuccessfully = false;

        static constexpr float MESSAGE_DISPLAY_TIME = 3.0f;
        static constexpr const char* FILTER_EXTENSION = ".tiles";
    };
}