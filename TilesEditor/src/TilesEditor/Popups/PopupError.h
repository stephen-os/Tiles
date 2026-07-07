#pragma once

#include "Popup.h"

#include <string>

namespace Tiles::Editor
{
    // Shared error/notification dialog. Any code raises it through
    // EditorHost::Notify, which sets the message and shows the popup, so failures
    // (save/load, etc.) surface from one reusable place instead of per-panel error
    // state.
    class PopupError : public Popup
    {
    public:
        PopupError(EditorHost& host);
        ~PopupError() = default;

        void SetMessage(const std::string& message) { m_Message = message; }

    protected:
        void OnRender() override;
        void OnUpdate() override {}

    private:
        std::string m_Message;
    };
}
