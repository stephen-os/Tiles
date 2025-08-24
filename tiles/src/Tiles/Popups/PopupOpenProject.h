#pragma once
#include "imgui.h"

#include "Core/Context.h"
#include "Core/Base.h"

#include <string>
#include <filesystem>

namespace Tiles
{
    class PopupOpenProject
    {
    public:
        PopupOpenProject();
        ~PopupOpenProject() = default;

        void Show(Ref<Context> context);
        void Render();
        bool IsVisible() const { return m_IsVisible; }
        void Close() { m_IsVisible = false; }

    private:
        void RenderOpenDialog();
        void RenderFileSettings();
        void RenderActionButtons();
        void ExecuteOpen();
        void ShowFileDialog();
        std::string GetFullFilePath() const;
        void ValidateFilePath();

    private:
        bool m_IsVisible = false;
        Ref<Context> m_Context;

        char m_FileName[256];
        std::string m_Directory;
        std::string m_Message;
        bool m_ShowMessage = false;
        bool m_ShowFileSelector = false;
        float m_MessageTimer = 0.0f;
        bool m_FilePathValid = false;

        static constexpr float MESSAGE_DISPLAY_TIME = 3.0f;
        static constexpr const char* FILTER_EXTENSION = ".tiles";
    };
}