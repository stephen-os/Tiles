#pragma once

#include "Popup.h"

namespace Tiles::Editor
{
    // Confirms quitting the application while one or more open documents have
    // unsaved changes. Raised when a window close is vetoed (see TilesEditorLayer).
    class PopupConfirmQuit : public Popup
    {
    public:
        PopupConfirmQuit(EditorHost& host);
        ~PopupConfirmQuit() = default;

    protected:
        void OnRender() override;
        void OnUpdate() override {}
    };
}
