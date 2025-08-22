#pragma once

#include "Panel.h"
#include "imgui.h"
#include <string>
#include <functional>

namespace Tiles
{
    class PanelMenuBar : public Panel
    {
    public:
        PanelMenuBar() = default;
        ~PanelMenuBar() = default;

        void Render() override;
        void Update() override;

        // Callback setters for external handling
        void SetNewProjectCallback(std::function<void()> callback) { m_NewProjectCallback = callback; }
        void SetOpenProjectCallback(std::function<void()> callback) { m_OpenProjectCallback = callback; }
        void SetSaveProjectCallback(std::function<void()> callback) { m_SaveProjectCallback = callback; }
        void SetSaveAsProjectCallback(std::function<void()> callback) { m_SaveAsProjectCallback = callback; }
        void SetExitCallback(std::function<void()> callback) { m_ExitCallback = callback; }
        void SetResizeProjectCallback(std::function<void()> callback) { m_ResizeProjectCallback = callback; }

    private:
        void RenderFileMenu();
        void RenderEditMenu();
        void RenderProjectMenu();
        void RenderViewMenu();
        void RenderHelpMenu();

        void ShowNewProjectDialog();
        void ShowResizeProjectDialog();
        void ShowAboutDialog();

        bool IsProjectLoaded() const;
        std::string GetProjectDisplayName() const;

    private:
        // Callbacks for external handling
        std::function<void()> m_NewProjectCallback;
        std::function<void()> m_OpenProjectCallback;
        std::function<void()> m_SaveProjectCallback;
        std::function<void()> m_SaveAsProjectCallback;
        std::function<void()> m_ExitCallback;
        std::function<void()> m_ResizeProjectCallback;

        // Dialog state
        bool m_ShowNewProjectDialog = false;
        bool m_ShowResizeProjectDialog = false;
        bool m_ShowAboutDialog = false;

        // New project dialog state
        char m_NewProjectName[128] = "New Project";
        int m_NewProjectWidth = 32;
        int m_NewProjectHeight = 32;

        // Resize project dialog state
        int m_ResizeWidth = 32;
        int m_ResizeHeight = 32;
    };
}