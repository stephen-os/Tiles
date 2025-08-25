#pragma once

#include "Popup.h"

#include <string>
#include <filesystem>

namespace Tiles
{
    class PopupSaveAs : public Popup
    {
    public:
        PopupSaveAs(Ref<Context> context);
        ~PopupSaveAs() = default;

    protected:
        void OnRender() override;
        void OnUpdate() override;

    private:
        void RenderBlockFileSettings();
        void RenderBlockActionButtons();
        void ValidateFileName();
        void ShowDirectoryDialog();
        std::string GetFullFilePath() const;
        void InitializeFromCurrentProject();

    private:
        std::string m_FileName = "Untitled";
        std::filesystem::path m_Directory = ".";
        std::string m_SaveMessage;

        bool m_ShowMessage = false;
        bool m_ShowDirectorySelector = false;
        float m_MessageTimer = 0.0f;
        bool m_FileNameValid = true;
        bool m_FirstShow = true;

        static constexpr float MESSAGE_DISPLAY_TIME = 3.0f;
    };
}