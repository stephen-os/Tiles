#pragma once

#include <cstddef>
#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include "Core/Error.h"
#include "Domain/Tile.h"
#include "Session/CommandDispatcher.h"
#include "Session/ProjectSession.h"
#include "Session/ViewportCameraController.h"
#include "Session/EditingState.h"

namespace Tiles
{
	// Owns the active project, the editing selection (working layer, brush,
	// painting mode), the undo/redo history, and the viewport camera, mediating
	// every edit through the command history. This is the single document seam
	// the editor drives via EditorHost::Doc().
	class Context
	{
	public:
		Context();
		~Context() = default;

		// Installs a this-capturing mutation hook, so it must not be copied or moved.
		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;
		Context(Context&&) = delete;
		Context& operator=(Context&&) = delete;

		// --- Viewport camera ---

		// The viewport camera.
		[[nodiscard]] Camera2D& GetViewportCamera() { return m_CameraController.GetCamera(); }
		[[nodiscard]] const Camera2D& GetViewportCamera() const { return m_CameraController.GetCamera(); }

		// Recenters the camera on the world origin at default zoom.
		void ResetViewportCamera();

		// Centers and zooms the camera so the whole project fits in view.
		void FitViewportCameraToProject();

		// Centers the camera on the world origin, keeping the current zoom.
		void CenterViewportCameraOnProject();

		// --- Editing selection ---

		// Selects the active layer for painting; ignored if index is out of range.
		void SetWorkingLayer(size_t index);

		// The index of the active painting layer.
		[[nodiscard]] size_t GetWorkingLayer() const { return m_EditingState.GetWorkingLayer(); }

		// True when the working-layer index refers to an existing layer.
		[[nodiscard]] bool HasWorkingLayer() const;

		// The active layer. Requires HasWorkingLayer(); asserts otherwise.
		[[nodiscard]] TileLayer& GetWorkingLayerRef();
		[[nodiscard]] const TileLayer& GetWorkingLayerRef() const;

		// Sets the active painting tool.
		void SetPaintingMode(PaintingMode mode) { m_EditingState.SetPaintingMode(mode); }

		// The active painting tool.
		[[nodiscard]] PaintingMode GetPaintingMode() const { return m_EditingState.GetPaintingMode(); }

		// Sets the tile stamped by the brush and fill tools.
		void SetBrush(const Tile& brush) { m_EditingState.SetBrush(brush); }

		// The tile stamped by the brush and fill tools.
		[[nodiscard]] const Tile& GetBrush() const { return m_EditingState.GetBrush(); }
		[[nodiscard]] Tile& GetBrush() { return m_EditingState.GetBrush(); }

		// --- Editing operations ---

		// Applies the current painting mode/brush to the working layer at (x, y).
		void PaintTile(int x, int y);

		// Dispatches the current painting mode (brush/eraser/fill) to the given
		// layer at (x, y), issuing the matching command. No-op for an invalid layer.
		void PaintTileOnLayer(size_t layerIndex, int x, int y, const Tile& tile);

		// Erases the tile on the working layer at (x, y).
		void EraseTile(int x, int y);

		// Flood-fills the working layer from (x, y), bounded by the visible view.
		void FillLayer(int x, int y, const glm::ivec4& bounds);

		// --- Undo / redo ---

		// Runs a command through the history (recording it for undo) and marks
		// the project modified. Null commands and a missing project are ignored.
		void ExecuteCommand(std::unique_ptr<Command> command);

		// True when there is a command to undo.
		[[nodiscard]] bool CanUndo() const { return m_CommandDispatcher.CanUndo(); }

		// True when there is a command to redo.
		[[nodiscard]] bool CanRedo() const { return m_CommandDispatcher.CanRedo(); }

		// Reverts the most recent command.
		void Undo();

		// Re-applies the most recently undone command.
		void Redo();

		// True while the project has unsaved changes.
		[[nodiscard]] bool IsDirty() const { return m_ProjectSession.IsDirty(); }

		// Drops the entire undo/redo history.
		void ClearHistory() { m_CommandDispatcher.Clear(); }

		// --- Project management ---

		// Replaces the current project with a fresh one, resetting brush,
		// painting mode, working layer, and command history.
		void CreateProject(const std::string& name);

		// Closes the active project, leaving the session empty and resetting the
		// editing selection, command history, and camera.
		void CloseProject();

		// Serializes the project to its existing file path.
		// @return Failure if there is no project or no path yet (use SaveProjectAs).
		[[nodiscard]] std::expected<void, Error> SaveProject();

		// Serializes the project to path and adopts it as the project's file path.
		[[nodiscard]] std::expected<void, Error> SaveProjectAs(const std::filesystem::path& path);

		// Loads a project from path, replacing the current one and resetting
		// editing state. A missing file is dropped from recent projects.
		[[nodiscard]] std::expected<void, Error> LoadProject(const std::filesystem::path& path);

		// True while a project is loaded.
		[[nodiscard]] bool HasProject() const { return m_ProjectSession.HasProject(); }

		// Project name, suffixed with "(Unsaved)" while it has no file path.
		[[nodiscard]] std::string GetProjectDisplayName() const { return m_ProjectSession.GetDisplayName(); }

		// The active project.
		[[nodiscard]] std::shared_ptr<Project> GetProject() const { return m_ProjectSession.GetProject(); }

		// --- Recent projects ---

		// The number of tracked recent projects.
		[[nodiscard]] size_t GetRecentProjectCount() const { return m_ProjectSession.GetRecentCount(); }

		// True while the recent-projects list is non-empty.
		[[nodiscard]] bool HasRecentProjects() const { return m_ProjectSession.HasRecent(); }

		// The recent-projects entry at index.
		[[nodiscard]] const ProjectHistoryEntry& GetRecentProject(size_t index) const { return m_ProjectSession.GetRecent(index); }

		// Clears the recent-projects list.
		void ClearRecentProjects() { m_ProjectSession.ClearRecent(); }

	private:
		// Clamps the working-layer index back into range after the layer count
		// changes (e.g. a delete via undo/redo).
		void ValidateWorkingLayer();

	private:
		ProjectSession m_ProjectSession;
		CommandDispatcher m_CommandDispatcher;
		ViewportCameraController m_CameraController;
		EditingState m_EditingState;
	};
}
