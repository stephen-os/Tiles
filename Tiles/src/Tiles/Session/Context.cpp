#include "Context.h"

namespace Tiles
{
	// Wires up the post-mutation bookkeeping and centers the camera.
	Context::Context()
	{
		// After any command runs, mark the project dirty and keep the working
		// layer in range (a layer delete can be undone/redone under us).
		m_CommandDispatcher.SetOnMutated([this]()
			{
				if (m_ProjectSession.HasProject())
					m_ProjectSession.GetProject()->MarkAsModified();
				ValidateWorkingLayer();
			});

		m_CameraController.Initialize();
	}

	// Recenters the camera on the world origin at default zoom.
	void Context::ResetViewportCamera()
	{
		m_CameraController.Initialize();
	}

	// Centers and zooms the camera so the whole project fits in view.
	void Context::FitViewportCameraToProject()
	{
		if (!m_ProjectSession.HasProject())
			return;

		const auto& layerStack = m_ProjectSession.GetProject()->GetLayerStack();
		m_CameraController.Fit(layerStack.GetBounds());
	}

	// Centers the camera on the world origin, keeping the current zoom.
	void Context::CenterViewportCameraOnProject()
	{
		m_CameraController.Center();
	}

	// Selects the active layer for painting; ignored if index is out of range.
	void Context::SetWorkingLayer(size_t index)
	{
		if (m_ProjectSession.HasProject()
			&& m_ProjectSession.GetProject()->GetLayerStack().IsValidLayerIndex(index))
			m_EditingState.SetWorkingLayer(index);
	}

	// True when the working-layer index refers to an existing layer.
	bool Context::HasWorkingLayer() const
	{
		return m_ProjectSession.HasProject()
			&& m_ProjectSession.GetProject()->GetLayerStack().IsValidLayerIndex(m_EditingState.GetWorkingLayer());
	}

	// The active layer. Requires HasWorkingLayer(); asserts otherwise.
	TileLayer& Context::GetWorkingLayerRef()
	{
		return m_ProjectSession.GetProject()->GetLayerStack().GetLayer(m_EditingState.GetWorkingLayer());
	}

	const TileLayer& Context::GetWorkingLayerRef() const
	{
		return m_ProjectSession.GetProject()->GetLayerStack().GetLayer(m_EditingState.GetWorkingLayer());
	}

	// Applies the current painting mode/brush to the working layer at (x, y).
	void Context::PaintTile(int x, int y)
	{
		if (HasWorkingLayer())
			PaintTileOnLayer(m_EditingState.GetWorkingLayer(), x, y, m_EditingState.GetBrush());
	}

	// Issues the current painting mode's command against the given layer.
	void Context::PaintTileOnLayer(size_t layerIndex, int x, int y, const Tile& tile)
	{
		if (!m_ProjectSession.HasProject()
			|| !m_ProjectSession.GetProject()->GetLayerStack().IsValidLayerIndex(layerIndex))
			return;

		ExecuteCommand(m_EditingState.BuildModeCommand(layerIndex, x, y, tile));
	}

	// Erases the tile on the working layer at (x, y).
	void Context::EraseTile(int x, int y)
	{
		if (HasWorkingLayer())
			ExecuteCommand(m_EditingState.BuildEraseCommand(m_EditingState.GetWorkingLayer(), x, y));
	}

	// Flood-fills the working layer from (x, y), bounded by the visible view.
	void Context::FillLayer(int x, int y, const glm::ivec4& bounds)
	{
		if (HasWorkingLayer())
			ExecuteCommand(m_EditingState.BuildFillCommand(m_EditingState.GetWorkingLayer(), x, y, m_EditingState.GetBrush(), bounds));
	}

	// Runs a command through the history, ignoring null commands / no project.
	void Context::ExecuteCommand(std::unique_ptr<Command> command)
	{
		if (m_ProjectSession.HasProject())
			m_CommandDispatcher.Execute(std::move(command), m_ProjectSession.GetProject()->GetLayerStack());
	}

	// Reverts the most recent command.
	void Context::Undo()
	{
		if (m_ProjectSession.HasProject())
			m_CommandDispatcher.Undo(m_ProjectSession.GetProject()->GetLayerStack());
	}

	// Re-applies the most recently undone command.
	void Context::Redo()
	{
		if (m_ProjectSession.HasProject())
			m_CommandDispatcher.Redo(m_ProjectSession.GetProject()->GetLayerStack());
	}

	// Clamps the working-layer index back into range against the active project.
	void Context::ValidateWorkingLayer()
	{
		if (!m_ProjectSession.HasProject())
			return;

		m_EditingState.ValidateWorkingLayer(m_ProjectSession.GetProject()->GetLayerStack());
	}

	// Replaces the current project with a fresh one, resetting editing/undo/camera.
	void Context::CreateProject(const std::string& name)
	{
		m_CommandDispatcher.Clear();
		m_ProjectSession.Create(name);
		m_EditingState.Reset();

		m_CameraController.Initialize();
	}

	// Closes the active project, resetting editing/undo/camera to the empty state.
	void Context::CloseProject()
	{
		m_CommandDispatcher.Clear();
		m_ProjectSession.Close();
		m_EditingState.Reset();

		m_CameraController.Initialize();
	}

	// Serializes the project to its existing file path.
	std::expected<void, Error> Context::SaveProject()
	{
		return m_ProjectSession.Save();
	}

	// Serializes the project to path and adopts it as the project's file path.
	std::expected<void, Error> Context::SaveProjectAs(const std::filesystem::path& path)
	{
		return m_ProjectSession.SaveAs(path);
	}

	// Loads a project from path, resetting editing/undo/camera around it.
	std::expected<void, Error> Context::LoadProject(const std::filesystem::path& path)
	{
		auto result = m_ProjectSession.Load(path);
		if (!result)
			return result;

		// A new project is in place; reset the editing/undo/camera state around it.
		m_CommandDispatcher.Clear();
		m_EditingState.Reset();
		ValidateWorkingLayer();

		m_CameraController.Initialize();

		return {};
	}
}
