#pragma once

#include "Popup.h"

namespace Tiles::Editor
{
    // Confirms closing a document that has unsaved changes: Save / Don't Save /
    // Cancel. It acts on the active document -- RequestCloseDocument switches to the
    // document being closed before raising this -- so it needs no state of its own.
    class PopupConfirmClose : public Popup
    {
    public:
        PopupConfirmClose(EditorHost& host);
        ~PopupConfirmClose() = default;

    protected:
        void OnRender() override;
        void OnUpdate() override {}
    };
}
