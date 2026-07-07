#pragma once

#include "Popup.h"

namespace Tiles::Editor
{
    // New-Project dialog: enter a name and create a fresh project via
    // Context::CreateProject, replacing the current one.
    class PopupNewProject : public Popup
    {
    public:
        PopupNewProject(EditorHost& host);
        ~PopupNewProject() = default;

    protected:
        void OnRender() override;
        void OnUpdate() override {}

    private:
        char m_NameBuffer[128] = "New Project";
    };
}
