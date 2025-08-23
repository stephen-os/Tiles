#pragma once
#include "Panel.h"
#include "imgui.h"
#include <string>

namespace Tiles
{
    class PanelMenuBar : public Panel
    {
    public:
        PanelMenuBar(Ref<Context> context);
        ~PanelMenuBar() = default;

        void Render() override;
        void Update() override;

    private:
        // Menu rendering methods
        void RenderFileMenu();
        void RenderEditMenu();
        void RenderProjectMenu();
        void RenderViewMenu();
        void RenderHelpMenu();

        // Dialog rendering methods
        void ShowNewProjectDialog();
        void ShowResizeProjectDialog();
        void ShowAboutDialog();
        void ShowFileDialog();

        // Helper methods
        void HandleKeyboardShortcuts();
        void OpenProject();
        void SaveProject();
        void SaveProjectAs();
        void CreateNewProject();
        void ResizeCurrentProject();

    private:
        // Dialog state
        bool m_ShowNewProjectDialog = false;
        bool m_ShowResizeProjectDialog = false;
        bool m_ShowAboutDialog = false;
        bool m_ShowOpenDialog = false;
        bool m_ShowSaveAsDialog = false;

        // New project dialog state
        char m_NewProjectName[128] = "New Project";
        int m_NewProjectWidth = 32;
        int m_NewProjectHeight = 32;

        // Resize project dialog state
        int m_ResizeWidth = 32;
        int m_ResizeHeight = 32;

        // File dialog state
        std::string m_CurrentFilePath;
        enum class FileDialogMode
        {
            None,
            Open,
            SaveAs
        } m_FileDialogMode = FileDialogMode::None;
    };
}