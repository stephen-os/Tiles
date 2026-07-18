#include "Session/Workspace.h"

#include "Domain/Project.h"

namespace Tiles
{
	// Starts with a single session so there is always an active document.
	Workspace::Workspace()
	{
		m_Sessions.push_back(std::make_unique<Session>());
	}

	// Replaces the active session's project with a fresh one.
	void Workspace::CreateProject(const std::string& name)
	{
		Active().CreateProject(name);
	}

	// Saves the active project in place, recording it in the recent list on success.
	std::expected<void, Error> Workspace::SaveProject()
	{
		auto result = Active().SaveProject();
		if (!result)
			return result;

		const Project& project = *Active().GetProject();
		m_Recent.AddProject(project.GetFilePath(), project.GetProjectName());
		return {};
	}

	// Saves the active project to a new path, recording it in the recent list.
	std::expected<void, Error> Workspace::SaveProjectAs(const std::filesystem::path& path)
	{
		auto result = Active().SaveProjectAs(path);
		if (!result)
			return result;

		const Project& project = *Active().GetProject();
		m_Recent.AddProject(project.GetFilePath(), project.GetProjectName());
		return {};
	}

	// Loads a project into the active session; a missing file is pruned from the
	// recent list rather than opened.
	std::expected<void, Error> Workspace::LoadProject(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			m_Recent.RemoveProject(path);
			return std::unexpected(Error{ ErrorCode::FileNotFound, "File does not exist." });
		}

		auto result = Active().LoadProject(path);
		if (!result)
			return result;

		const Project& project = *Active().GetProject();
		m_Recent.AddProject(project.GetFilePath(), project.GetProjectName());
		return {};
	}
}
