#pragma once
#include "Popup.h"
#include <string>
#include <filesystem>

namespace Tiles
{
    class PopupOpenProject : public Popup
    {
    public:
        PopupOpenProject(Ref<Context> context);
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
        std::string m_FileName;
        std::filesystem::path m_Directory = ".";
        std::string m_Message;
        bool m_ShowMessage = false;
        bool m_ShowFileSelector = false;
        float m_MessageTimer = 0.0f;
        bool m_FilePathValid = false;
        bool m_FirstShow = true;

        static constexpr float MESSAGE_DISPLAY_TIME = 3.0f;
        static constexpr const char* FILTER_EXTENSION = ".tiles";
    };
}