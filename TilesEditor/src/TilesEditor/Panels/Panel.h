#pragma once

#include "Session/Session.h"

#include "Tiles.h"

#include "../App/EditorHost.h"

namespace Tiles::Editor
{
    // Base class for editor panels: a dockable ImGui window driven once per frame
    // by PanelManager (Update() first, then Render()). A panel borrows the shared
    // document and its sibling views through the EditorHost facade -- it owns
    // neither -- so the host is the single seam every panel reaches state through.
    // m_Open backs the window's show/hide state so panels can be closed and
    // reopened Visual-Studio-style via the View menu.
    class Panel
    {
    public:
        Panel(EditorHost& host) : m_Host(host) {}
        virtual ~Panel() = default;

        virtual void Render() = 0;
        virtual void Update() = 0;

        bool IsOpen() const { return m_Open; }
        void SetOpen(bool open) { m_Open = open; }
        // Backing flag to pass to ImGui::Begin so the window's close button and the
        // View-menu toggle drive the same state.
        bool* OpenFlag() { return &m_Open; }

    protected:
        EditorHost& Host() const { return m_Host; }
        // Shorthand for the active document's session state.
        Session& Ctx() const { return m_Host.Doc(); }
        // Shorthand for the app-level workspace (recent projects, open/save).
        Workspace& Space() const { return m_Host.Space(); }

        EditorHost& m_Host;
        bool m_Open = true;
    };
}
