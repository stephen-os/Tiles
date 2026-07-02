#pragma once
#include "Panel.h"
#include "imgui.h"
#include <string>

#include "../Popups/PopupRenderMatrix.h"
#include "../Popups/PopupSaveAs.h"
#include "../Popups/PopupOpenProject.h"

namespace Tiles
{
    /// The application's main menu bar (File/Edit/Project/View/Help), the source
    /// of the global keyboard shortcuts, and the host for the New/Resize/About
    /// modal dialogs plus the Save-As, Open, and Export (render matrix) popups.
    class PanelMenuBar : public Panel
    {
    public:
        PanelMenuBar(std::shared_ptr<Context> context);
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
        void CreateNewProject();
        void ResizeCurrentProject();

        // Raises the error dialog when a save/load result reports failure.
        void ReportResult(const ProjectResult& result);
        void RenderErrorDialog();

    private:
		PopupRenderMatrix m_PopupRenderMatrix;
        PopupSaveAs m_PopupSaveAs;
        PopupOpenProject m_PopupOpenProject;

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

        // Error dialog state
        bool m_ShowErrorDialog = false;
        std::string m_ErrorMessage;
    };
}