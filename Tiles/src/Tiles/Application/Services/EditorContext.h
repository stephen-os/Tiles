#pragma once

#include "ProjectService.h"
#include "PaintingService.h"
#include "CommandHistory.h"
#include "../Interfaces/IRenderer.h"

#include <memory>
#include <functional>

namespace Tiles::Services
{
    // Application context that coordinates all services
    // This is the main entry point for the application layer
    // Replaces the monolithic Context class with a composition of services
    class EditorContext
    {
    public:
        EditorContext(
            std::shared_ptr<Domain::IProjectRepository> projectRepository,
            std::shared_ptr<ICamera> viewportCamera
        );
        ~EditorContext() = default;

        // Service access
        ProjectService& GetProjectService() { return m_ProjectService; }
        const ProjectService& GetProjectService() const { return m_ProjectService; }

        PaintingService& GetPaintingService() { return m_PaintingService; }
        const PaintingService& GetPaintingService() const { return m_PaintingService; }

        CommandHistory& GetCommandHistory() { return m_CommandHistory; }
        const CommandHistory& GetCommandHistory() const { return m_CommandHistory; }

        // Camera access
        std::shared_ptr<ICamera> GetViewportCamera() { return m_ViewportCamera; }
        std::shared_ptr<const ICamera> GetViewportCamera() const { return m_ViewportCamera; }

        // Convenience methods that delegate to services
        bool HasProject() const { return m_ProjectService.HasProject(); }
        Domain::TileProject* GetProject() { return m_ProjectService.GetCurrentProject(); }
        const Domain::TileProject* GetProject() const { return m_ProjectService.GetCurrentProject(); }

        bool CanUndo() const { return m_CommandHistory.CanUndo(); }
        bool CanRedo() const { return m_CommandHistory.CanRedo(); }
        void Undo() { m_CommandHistory.Undo(); }
        void Redo() { m_CommandHistory.Redo(); }

        // Factory method
        static std::shared_ptr<EditorContext> Create(
            std::shared_ptr<Domain::IProjectRepository> projectRepository,
            std::shared_ptr<ICamera> viewportCamera
        );

    private:
        CommandHistory m_CommandHistory;
        ProjectService m_ProjectService;
        PaintingService m_PaintingService;
        std::shared_ptr<ICamera> m_ViewportCamera;
    };
}
