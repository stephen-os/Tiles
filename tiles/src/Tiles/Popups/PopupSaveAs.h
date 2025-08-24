#pragma once

#include "imgui.h"

#include "Core/Context.h"
#include "Core/Base.h"

#include <string>
#include <filesystem>

namespace Tiles
{
    class PopupSaveAs
    {
    public:
        PopupSaveAs();
        ~PopupSaveAs() = default;

        void Show(Ref<Context> context);
        void Render();
        bool IsVisible() const { return m_IsVisible; }
        void Close() { m_IsVisible = false; }

    private:
        void RenderSaveAsDialog();
        void RenderFileSettings();
        void RenderActionButtons();
        void ExecuteSaveAs();
        void ValidateFileName();
        void ShowDirectoryDialog();
        std::string GetFullFilePath() const;

    private:
        bool m_IsVisible = false;
        Ref<Context> m_Context;

        char m_FileName[256];
        std::string m_Directory;
        std::string m_SaveMessage;
        bool m_ShowMessage = false;
        bool m_ShowDirectorySelector = false;
        float m_MessageTimer = 0.0f;
        bool m_FileNameValid = true;

        static constexpr float MESSAGE_DISPLAY_TIME = 3.0f;
        static constexpr const char* DEFAULT_EXTENSION = ".tiles";
    };
}