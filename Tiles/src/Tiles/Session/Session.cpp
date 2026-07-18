#include "Session.h"

#include "Core/Logger.h"

namespace Tiles
{
	// Starts a session on a fresh "Untitled" project and wires post-mutation bookkeeping.
	Session::Session()
		: m_Project(std::make_shared<Project>("Untitled"))
	{
		m_Project->UpdateLastAccessed();

		// After any command runs, mark the project dirty and keep the working
		// layer in range (a layer delete can be undone/redone under us).
		m_CommandDispatcher.SetOnMutated([this]()
			{
				m_Project->MarkAsModified();
				ValidateWorkingLayer();
			});

		m_Camera.Reset();
	}

	// Selects the active layer for painting; ignored if index is out of range.
	void Session::SetWorkingLayer(size_t index)
	{
		if (m_Project->GetLayerStack().IsValidLayerIndex(index))
			m_EditingState.SetWorkingLayer(index);
	}

	// True when the working-layer index refers to an existing layer.
	bool Session::HasWorkingLayer() const
	{
		return m_Project->GetLayerStack().IsValidLayerIndex(m_EditingState.GetWorkingLayer());
	}

	// The active layer. Requires HasWorkingLayer(); asserts otherwise.
	TileLayer& Session::GetWorkingLayerRef()
	{
		return m_Project->GetLayerStack().GetLayer(m_EditingState.GetWorkingLayer());
	}

	const TileLayer& Session::GetWorkingLayerRef() const
	{
		return m_Project->GetLayerStack().GetLayer(m_EditingState.GetWorkingLayer());
	}

	// Flood-fills the working layer from (x, y), bounded by the visible view.
	void Session::FillLayer(int x, int y, const glm::ivec4& bounds)
	{
		if (HasWorkingLayer())
			ExecuteCommand(m_EditingState.BuildFillCommand(m_EditingState.GetWorkingLayer(), x, y, m_EditingState.GetBrush(), bounds));
	}

	// The cells the brush covers when centered on (cx, cy).
	std::vector<glm::ivec2> Session::GetBrushFootprint(int cx, int cy) const
	{
		return m_EditingState.BrushFootprint(cx, cy);
	}

	// Paints the given cells as one undoable stroke, using the current mode/brush.
	void Session::PaintStroke(const std::vector<glm::ivec2>& cells)
	{
		if (HasWorkingLayer())
			ExecuteCommand(m_EditingState.BuildStrokeCommand(m_EditingState.GetWorkingLayer(), cells));
	}

	// Places the current stamp centered on anchor as one undoable block.
	void Session::PaintStamp(const glm::ivec2& anchor)
	{
		if (HasWorkingLayer())
			ExecuteCommand(m_EditingState.BuildStampCommand(m_EditingState.GetWorkingLayer(), anchor));
	}

	// Moves the given working-layer cells by offset as one undoable step.
	void Session::MoveSelection(const std::vector<glm::ivec2>& cells, const glm::ivec2& offset)
	{
		if (HasWorkingLayer())
			ExecuteCommand(m_EditingState.BuildMoveCommand(m_EditingState.GetWorkingLayer(), cells, offset, m_Project->GetLayerStack()));
	}

	// The cells of the current shape tool between start and end.
	std::vector<glm::ivec2> Session::GetShapeCells(const glm::ivec2& start, const glm::ivec2& end) const
	{
		return m_EditingState.ShapeCells(start, end);
	}

	// Runs a command through the history against the project's layer stack.
	void Session::ExecuteCommand(std::unique_ptr<Command> command)
	{
		m_CommandDispatcher.Execute(std::move(command), m_Project->GetLayerStack());
	}

	// Reverts the most recent command.
	void Session::Undo()
	{
		m_CommandDispatcher.Undo(m_Project->GetLayerStack());
	}

	// Re-applies the most recently undone command.
	void Session::Redo()
	{
		m_CommandDispatcher.Redo(m_Project->GetLayerStack());
	}

	// Clamps the working-layer index back into range against the active project.
	void Session::ValidateWorkingLayer()
	{
		m_EditingState.ValidateWorkingLayer(m_Project->GetLayerStack());
	}

	// Project name, suffixed with "(Unsaved)" while it has no file path.
	std::string Session::GetProjectDisplayName() const
	{
		std::string name = m_Project->GetProjectName();
		if (m_Project->IsNew())
			name += " (Unsaved)";

		return name;
	}

	// Replaces the current project with a fresh one, resetting editing/undo/camera.
	void Session::CreateProject(const std::string& name)
	{
		m_CommandDispatcher.Clear();

		m_Project = std::make_shared<Project>(name);
		m_Project->UpdateLastAccessed();

		m_EditingState.Reset();
		m_Camera.Reset();
	}

	// Serializes the project to its existing file path; fails if it has none yet.
	std::expected<void, Error> Session::SaveProject()
	{
		if (m_Project->IsNew())
			return std::unexpected(Error{ ErrorCode::NoFilePath, "Project has no file path. Use 'Save As' to specify a location." });

		const auto path = m_Project->GetFilePath();

		auto result = ProjectSerializer::Save(*m_Project, path);
		if (!result)
			return result;

		m_Project->MarkAsSaved();
		m_Project->UpdateLastAccessed();
		m_History.AddProject(path, m_Project->GetProjectName());

		TILES_ENGINE_INFO("Session::SaveProject: Successfully saved project '{}'", m_Project->GetProjectName());
		return {};
	}

	// Serializes the project to path and adopts it as the project's file path.
	std::expected<void, Error> Session::SaveProjectAs(const std::filesystem::path& path)
	{
		if (path.empty())
			return std::unexpected(Error{ ErrorCode::InvalidPath, "Invalid file path." });

		auto result = ProjectSerializer::Save(*m_Project, path);
		if (!result)
			return result;

		m_Project->SetFilePath(path.string());
		m_Project->MarkAsSaved();
		m_Project->UpdateLastAccessed();
		m_History.AddProject(path, m_Project->GetProjectName());

		TILES_ENGINE_INFO("Session::SaveProjectAs: Successfully saved project '{}' to '{}'", m_Project->GetProjectName(), path.string());
		return {};
	}

	// Loads a project from path, resetting editing/undo/camera around it; a
	// missing file is dropped from recent projects and reported as a failure.
	std::expected<void, Error> Session::LoadProject(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			TILES_ENGINE_INFO("Session::LoadProject: File does not exist: {}", path.string());
			m_History.RemoveProject(path);
			return std::unexpected(Error{ ErrorCode::FileNotFound, "File does not exist." });
		}

		auto result = ProjectSerializer::Load(path);
		if (!result)
		{
			TILES_ENGINE_INFO("Session::LoadProject: Load failed: {}", result.error().message);
			return std::unexpected(result.error());
		}

		m_Project = *result;
		m_Project->SetFilePath(path.string());
		m_Project->MarkAsSaved();
		m_Project->UpdateLastAccessed();
		m_History.AddProject(path, m_Project->GetProjectName());

		// A new project is in place; reset the editing/undo/camera state around it.
		m_CommandDispatcher.Clear();
		m_EditingState.Reset();
		ValidateWorkingLayer();
		m_Camera.Reset();

		TILES_ENGINE_INFO("Session::LoadProject: Successfully loaded project '{}' from '{}'", m_Project->GetProjectName(), path.string());
		return {};
	}
}
