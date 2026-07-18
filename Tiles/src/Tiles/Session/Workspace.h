#pragma once

#include <expected>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "Core/Error.h"
#include "Session/Session.h"
#include "Session/ProjectHistory.h"

namespace Tiles
{
	// The app-level editing workspace: it owns the open editing sessions (one per
	// project) and the app-wide recent-projects list, and orchestrates the project
	// lifecycle (new / open / save) across the active session. The editor reaches it
	// via EditorHost::Space(); per-document state is reached through the active
	// Session (EditorHost::Doc()).
	//
	// Single-document for now: exactly one session, always active. The N-session
	// collection and switching land in a later step.
	class Workspace
	{
	public:
		// Starts with one fresh session (an "Untitled" project).
		Workspace();
		~Workspace() = default;

		// Owns unique sessions; not copyable.
		Workspace(const Workspace&) = delete;
		Workspace& operator=(const Workspace&) = delete;

		// The active editing session -- the document panels read and mutate.
		[[nodiscard]] Session& Active() { return *m_Sessions[m_Active]; }
		[[nodiscard]] const Session& Active() const { return *m_Sessions[m_Active]; }

		// --- Open documents ---

		[[nodiscard]] size_t DocumentCount() const { return m_Sessions.size(); }
		[[nodiscard]] size_t ActiveIndex() const { return m_Active; }
		[[nodiscard]] Session& DocumentAt(size_t index) { return *m_Sessions[index]; }
		[[nodiscard]] const Session& DocumentAt(size_t index) const { return *m_Sessions[index]; }

		// Makes the document at index active; ignored if out of range.
		void SwitchTo(size_t index);

		// Opens a fresh blank document (an "Untitled" project) as a new active tab.
		void NewDocument();

		// --- Project lifecycle (drives the active session + recent list) ---

		// Replaces the active session's project with a fresh one.
		void CreateProject(const std::string& name);

		// Saves the active project in place, recording it as recent on success.
		// @return NoFilePath if the project has never been saved (use SaveProjectAs).
		[[nodiscard]] std::expected<void, Error> SaveProject();

		// Saves the active project to path, adopts it, and records it as recent.
		[[nodiscard]] std::expected<void, Error> SaveProjectAs(const std::filesystem::path& path);

		// Loads a project from path into the active session, recording it as recent.
		// A missing file is dropped from the recent list and reported as a failure.
		[[nodiscard]] std::expected<void, Error> LoadProject(const std::filesystem::path& path);

		// --- Recent projects ---

		[[nodiscard]] size_t GetRecentProjectCount() const { return m_Recent.GetCount(); }
		[[nodiscard]] bool HasRecentProjects() const { return !m_Recent.IsEmpty(); }
		[[nodiscard]] const ProjectHistoryEntry& GetRecentProject(size_t index) const { return m_Recent.GetEntry(index); }
		void ClearRecentProjects() { m_Recent.Clear(); }

	private:
		// True when the active document is an untouched, never-saved "Untitled"
		// project -- a New/Open reuses it instead of adding a blank tab.
		[[nodiscard]] bool ActiveIsPristine() const;

		std::vector<std::unique_ptr<Session>> m_Sessions;
		size_t m_Active = 0;
		ProjectHistory m_Recent;
	};
}
