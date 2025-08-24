#pragma once

#include "imgui.h"
#include "../Core/Context.h"
#include "../Core/Base.h"

#include <string>

namespace Tiles
{
    class PopupSave
    {
    public:
        PopupSave();
        ~PopupSave() = default;

        void Show(Ref<Context> context);
        void Render();
        bool IsVisible() const { return m_IsVisible; }
        void Close() { m_IsVisible = false; }

    private:
        void RenderSaveDialog();
        void RenderActionButtons();
        void ExecuteSave();

    private:
        bool m_IsVisible = false;
        Ref<Context> m_Context;
        std::string m_SaveMessage;
        bool m_ShowMessage = false;
        float m_MessageTimer = 0.0f;

        static constexpr float MESSAGE_DISPLAY_TIME = 3.0f;
    };
}
