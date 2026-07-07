#pragma once
#include "Panel.h"
#include "imgui.h"

namespace Tiles::Editor
{
    // The application's main menu bar (File/Edit/Project/View/Help). Menu items
    // fire editor actions and open dialogs through the host facade and toggle
    // panels by id; it owns no popups and reads no keyboard shortcuts of its own
    // (global shortcuts live in the editor's action/keybind map).
    class PanelMenuBar : public Panel
    {
    public:
        PanelMenuBar(EditorHost& host) : Panel(host) {}
        ~PanelMenuBar() = default;

        void Render() override;
        void Update() override {}

    private:
        void RenderFileMenu();
        void RenderEditMenu();
        void RenderProjectMenu();
        void RenderViewMenu();
        void RenderHelpMenu();
    };
}
