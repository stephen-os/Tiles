#pragma once

#include <filesystem>

#include "Tile.h"
#include "Project.h"
#include "CommandHistory.h"
#include "ProjectHistory.h"
#include "ProjectSerializer.h"

#include "Constants.h"

#include "../Tiles.h"

#include "Base.h"

namespace Tiles
{
    enum class PaintingMode
    {
        None = 0,
        Brush,
        Eraser,
        Fill
    };

    /// Owns the active project and editing state (working layer, brush,
    /// painting mode) and mediates every edit through the undo/redo history.
    class Context
    {
    public:
        static std::shared_ptr<Context> Create();

        Context();
        ~Context() = default;

        std::shared_ptr<Tiles::OrthographicCamera> GetViewportCamera() { return m_ViewportCamera; }
        const std::shared_ptr<Tiles::OrthographicCamera> GetViewportCamera() const { return m_ViewportCamera; }
        /// Recenters the camera on the project at default zoom.
        void ResetViewportCamera();
        /// Centers and zooms the camera so the whole project fits in view.
        void FitViewportCameraToProject();
        void CenterViewportCameraOnProject();

        /// Selects the active layer for painting; ignored if index is out of range.
        void SetWorkingLayer(size_t index);
        size_t GetWorkingLayer() const { return m_WorkingLayer; }
        bool HasWorkingLayer() const;
        /// Returns the active layer. Requires HasWorkingLayer(); asserts otherwise.
        TileLayer& GetWorkingLayerRef();
        const TileLayer& GetWorkingLayerRef() const;

        void SetPaintingMode(PaintingMode mode) { m_PaintingMode = mode; }
        PaintingMode GetPaintingMode() const { return m_PaintingMode; }
        void SetBrush(const Tile& brush) { m_Brush = brush; }
        const Tile& GetBrush() const { return m_Brush; }
        Tile& GetBrush() { return m_Brush; }

        /// Applies the current painting mode/brush to the working layer at (x, y).
        void PaintTile(size_t x, size_t y);
        /// Dispatches the current painting mode (brush/eraser/fill) to the given
        /// layer at (x, y), issuing the matching command. No-op for an invalid layer.
        void PaintTileOnLayer(size_t layerIndex, size_t x, size_t y, const Tile& tile);
        void EraseTile(size_t x, size_t y);
        void FillLayer(size_t x, size_t y);

        /// Runs a command through the history (recording it for undo) and marks
        /// the project modified. Null commands and a missing project are ignored.
        void ExecuteCommand(std::unique_ptr<Command> command);
        bool CanUndo() const { return m_CommandHistory.CanUndo(); }
        bool CanRedo() const { return m_CommandHistory.CanRedo(); }
        void Undo();
        void Redo();

        bool IsDirty() const { return m_Project->HasUnsavedChanges(); }

		void ClearHistory() { m_CommandHistory.Clear(); }   

		// Project Management

        /// Replaces the current project with a fresh one, resetting brush,
        /// painting mode, working layer, and command history.
        void CreateProject(const std::string& name, uint32_t width, uint32_t height);
        /// Serializes the project to its existing file path.
        /// @return Failure if there is no project or no path yet (use SaveProjectAs).
        ProjectResult SaveProject();
        /// Serializes the project to path and adopts it as the project's file path.
        ProjectResult SaveProjectAs(const std::filesystem::path& path);
        /// Loads a project from path, replacing the current one and resetting
        /// editing state. A missing file is dropped from recent projects.
        ProjectResult LoadProject(const std::filesystem::path& path);
        /// Resizes the grid, keeping the camera focused on the same relative point.
        void ResizeProject(uint32_t width, uint32_t height);
        bool HasProject() const { return m_Project != nullptr; }
        /// Project name, suffixed with "(Unsaved)" while it has no file path.
        std::string GetProjectDisplayName() const;
        std::shared_ptr<Project> GetProject() { return m_Project; }
        const std::shared_ptr<Project> GetProject() const { return m_Project; }

        // Project History
        size_t GetRecentProjectCount() const { return m_ProjectHistory.GetCount(); }
        bool HasRecentProjects() const { return !m_ProjectHistory.IsEmpty(); }
        const ProjectHistoryEntry& GetRecentProject(size_t index) const { return m_ProjectHistory.GetEntry(index); }
        void ClearRecentProjects() { m_ProjectHistory.Clear(); }

    private:
        /// Clamps the working-layer index back into range after the layer count
        /// changes (e.g. a delete via undo/redo).
        void ValidateWorkingLayer();
        /// Positions the camera at the project's center at default zoom.
        void InitializeSceneCamera();

    private:
        CommandHistory m_CommandHistory;
        ProjectHistory m_ProjectHistory;

        std::shared_ptr<Project> m_Project;

        std::shared_ptr<Tiles::OrthographicCamera> m_ViewportCamera;
        
        size_t m_WorkingLayer = 0;
        PaintingMode m_PaintingMode = PaintingMode::None;
        Tile m_Brush;
    };
}