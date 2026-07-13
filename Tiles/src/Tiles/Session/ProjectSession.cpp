#include "ProjectSession.h"
#include "Core/Logger.h"

namespace Tiles
{
	// Starts with a fresh, unsaved "Untitled" project.
	ProjectSession::ProjectSession()
		: m_Project(std::make_shared<Project>("Untitled"))
	{
		m_Project->UpdateLastAccessed();
	}

	// Project name, suffixed with "(Unsaved)" while it has no file path.
	std::string ProjectSession::GetDisplayName() const
	{
		if (!m_Project)
			return "No Project";

		std::string name = m_Project->GetProjectName();
		if (m_Project->IsNew())
			name += " (Unsaved)";

		return name;
	}

	// Replaces the project with a fresh, empty one.
	void ProjectSession::Create(const std::string& name)
	{
		m_Project = std::make_shared<Project>(name);
		m_Project->UpdateLastAccessed();
	}

	// Serializes to the project's existing file path; fails if it has none yet.
	ProjectResult ProjectSession::Save()
	{
		if (!m_Project)
			return { false, "No project loaded." };

		if (m_Project->IsNew())
			return { false, "Project has no file path. Use 'Save As' to specify a location." };

		const auto path = m_Project->GetFilePath();

		ProjectResult result = ProjectSerializer::Save(*m_Project, path);
		if (!result.Success)
			return result;

		m_Project->MarkAsSaved();
		m_Project->UpdateLastAccessed();
		m_History.AddProject(path, m_Project->GetProjectName());

		TILES_ENGINE_INFO("ProjectSession::Save: Successfully saved project '{}'", m_Project->GetProjectName());
		return result;
	}

	// Serializes to path and adopts it as the project's file path.
	ProjectResult ProjectSession::SaveAs(const std::filesystem::path& path)
	{
		if (!m_Project)
			return { false, "No project loaded." };

		if (path.empty())
			return { false, "Invalid file path." };

		ProjectResult result = ProjectSerializer::Save(*m_Project, path);
		if (!result.Success)
			return result;

		m_Project->SetFilePath(path.string());
		m_Project->MarkAsSaved();
		m_Project->UpdateLastAccessed();
		m_History.AddProject(path, m_Project->GetProjectName());

		TILES_ENGINE_INFO("ProjectSession::SaveAs: Successfully saved project '{}' to '{}'", m_Project->GetProjectName(), path.string());
		return result;
	}

	// Loads path into the active project, recording it in the history; a missing
	// file is dropped from the history and reported as a failure.
	ProjectResult ProjectSession::Load(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			TILES_ENGINE_INFO("ProjectSession::Load: File does not exist: {}", path.string());
			m_History.RemoveProject(path);
			return { false, "File does not exist." };
		}

		std::shared_ptr<Project> project;
		ProjectResult result = ProjectSerializer::Load(path, project);
		if (!result.Success)
		{
			TILES_ENGINE_INFO("ProjectSession::Load: Load failed: {}", result.Message);
			return result;
		}

		project->SetFilePath(path.string());
		project->MarkAsSaved();
		project->UpdateLastAccessed();

		m_Project = project;
		m_History.AddProject(path, m_Project->GetProjectName());

		TILES_ENGINE_INFO("ProjectSession::Load: Successfully loaded project '{}' from '{}'", m_Project->GetProjectName(), path.string());
		return result;
	}
}
