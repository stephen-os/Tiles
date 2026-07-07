#pragma once

#include "Popup.h"

namespace Tiles::Editor
{
    // Static "About Tiles" dialog: version and a short feature list.
    class PopupAbout : public Popup
    {
    public:
        PopupAbout(EditorHost& host);
        ~PopupAbout() = default;

    protected:
        void OnRender() override;
        void OnUpdate() override {}
    };
}
