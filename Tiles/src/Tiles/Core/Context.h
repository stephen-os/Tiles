#pragma once
#include <memory>
#include <string>
#include <cstdint>

#include <filesystem>

#include "Domain/Tile.h"
#include "Session/CommandDispatcher.h"
#include "Session/ProjectSession.h"

#include "Constants.h"

#include "Session/ViewportCameraController.h"
#include "Session/EditingState.h"


namespace Tiles
{
    /// Owns the active project and editing state (working layer, brush,
    /// painting mode) and mediates every edit through the undo/redo history.
    class Context
    {
    public:
        static std::shared_ptr<Context> Create();

        Context();
        ~Context() = default;

        Camera2D& GetViewportCamera() { return m_CameraController.GetCamera(); }
        const Camera2D& GetViewportCamera() const { return m_CameraController.GetCamera(); }
        /// Recenters the camera on the project at default zoom.
        void ResetViewportCamera();
        /// Centers and zooms the camera so the whole project fits in view.
        void FitViewportCameraToProject();
        void CenterViewportCameraOnProject();

        /// Selects the active layer for painting; ignored if index is out of range.
        void SetWorkingLayer(size_t index);
        size_t GetWorkingLayer() const { return m_EditingState.GetWorkingLayer(); }
        bool HasWorkingLayer() const;
        /// Returns the active layer. Requires HasWorkingLayer(); asserts otherwise.
        TileLayer& GetWorkingLayerRef();
        const TileLayer& GetWorkingLayerRef() const;

        void SetPaintingMode(PaintingMode mode) { m_EditingState.SetPaintingMode(mode); }
        PaintingMode GetPaintingMode() const { return m_EditingState.GetPaintingMode(); }
        void SetBrush(const Tile& brush) { m_EditingState.SetBrush(brush); }
        const Tile& GetBrush() const { return m_EditingState.GetBrush(); }
        Tile& GetBrush() { return m_EditingState.GetBrush(); }

        /// Applies the current painting mode/brush to the working layer at (x, y).
        void PaintTile(int x, int y);
        /// Dispatches the current painting mode (brush/eraser/fill) to the given
        /// layer at (x, y), issuing the matching command. No-op for an invalid layer.
        void PaintTileOnLayer(size_t layerIndex, int x, int y, const Tile& tile);
        void EraseTile(int x, int y);
        void FillLayer(int x, int y);

        /// Runs a command through the history (recording it for undo) and marks
        /// the project modified. Null commands and a missing project are ignored.
        void ExecuteCommand(std::unique_ptr<Command> command);
        bool CanUndo() const { return m_CommandDispatcher.CanUndo(); }
        bool CanRedo() const { return m_CommandDispatcher.CanRedo(); }
        void Undo();
        void Redo();

        bool IsDirty() const { return m_ProjectSession.IsDirty(); }

		void ClearHistory() { m_CommandDispatcher.Clear(); }

		// Project Management

        /// Replaces the current project with a fresh one, resetting brush,
        /// painting mode, working layer, and command history.
        void CreateProject(const std::string& name);
        /// Serializes the project to its existing file path.
        /// @return Failure if there is no project or no path yet (use SaveProjectAs).
        ProjectResult SaveProject();
        /// Serializes the project to path and adopts it as the project's file path.
        ProjectResult SaveProjectAs(const std::filesystem::path& path);
        /// Loads a project from path, replacing the current one and resetting
        /// editing state. A missing file is dropped from recent projects.
        ProjectResult LoadProject(const std::filesystem::path& path);
        bool HasProject() const { return m_ProjectSession.HasProject(); }
        /// Project name, suffixed with "(Unsaved)" while it has no file path.
        std::string GetProjectDisplayName() const { return m_ProjectSession.GetDisplayName(); }
        std::shared_ptr<Project> GetProject() { return m_ProjectSession.GetProject(); }
        const std::shared_ptr<Project> GetProject() const { return m_ProjectSession.GetProject(); }

        // Project History
        size_t GetRecentProjectCount() const { return m_ProjectSession.GetRecentCount(); }
        bool HasRecentProjects() const { return m_ProjectSession.HasRecent(); }
        const ProjectHistoryEntry& GetRecentProject(size_t index) const { return m_ProjectSession.GetRecent(index); }
        void ClearRecentProjects() { m_ProjectSession.ClearRecent(); }

    private:
        /// Clamps the working-layer index back into range after the layer count
        /// changes (e.g. a delete via undo/redo).
        void ValidateWorkingLayer();

    private:
        ProjectSession m_ProjectSession;
        CommandDispatcher m_CommandDispatcher;
        ViewportCameraController m_CameraController;
        EditingState m_EditingState;
    };
}