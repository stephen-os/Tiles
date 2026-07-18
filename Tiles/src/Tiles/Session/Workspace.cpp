#include "Session/Workspace.h"

#include "Domain/Project.h"

namespace Tiles
{
	// Starts with a single session so there is always an active document.
	Workspace::Workspace()
	{
		m_Sessions.push_back(std::make_unique<Session>());
	}

	// Opens a named project as a new tab, reusing a pristine active tab when there is one.
	void Workspace::CreateProject(const std::string& name)
	{
		if (!ActiveIsPristine())
		{
			m_Sessions.push_back(std::make_unique<Session>());
			m_Active = m_Sessions.size() - 1;
		}

		Active().CreateProject(name);
	}

	// Makes the document at index active.
	void Workspace::SwitchTo(size_t index)
	{
		if (index < m_Sessions.size())
			m_Active = index;
	}

	// Opens a fresh blank document ("Untitled") as a new active tab.
	void Workspace::NewDocument()
	{
		m_Sessions.push_back(std::make_unique<Session>());
		m_Active = m_Sessions.size() - 1;
	}

	// Closes a document; the last one resets to a fresh "Untitled" so the workspace
	// is never empty.
	void Workspace::CloseDocument(size_t index)
	{
		if (index >= m_Sessions.size())
			return;

		if (m_Sessions.size() == 1)
		{
			m_Sessions[0] = std::make_unique<Session>();
			m_Active = 0;
			return;
		}

		m_Sessions.erase(m_Sessions.begin() + index);

		// Keep the active index valid: shift down if it was past the removed one, or
		// clamp to the new last document if the active one was itself the last.
		if (m_Active > index)
			--m_Active;
		else if (m_Active == index && m_Active >= m_Sessions.size())
			m_Active = m_Sessions.size() - 1;
	}

	// True when the active document is a never-saved, unmodified "Untitled".
	bool Workspace::ActiveIsPristine() const
	{
		const Project& project = *Active().GetProject();
		return project.IsNew() && !project.HasUnsavedChanges();
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

		if (ActiveIsPristine())
		{
			// Reuse the empty active tab.
			auto result = Active().LoadProject(path);
			if (!result)
				return result;
		}
		else
		{
			// Load into a new session first; only adopt it as a tab on success, so a
			// bad file never leaves a broken tab behind.
			auto session = std::make_unique<Session>();
			auto result = session->LoadProject(path);
			if (!result)
				return result;

			m_Sessions.push_back(std::move(session));
			m_Active = m_Sessions.size() - 1;
		}

		const Project& project = *Active().GetProject();
		m_Recent.AddProject(project.GetFilePath(), project.GetProjectName());
		return {};
	}
}
