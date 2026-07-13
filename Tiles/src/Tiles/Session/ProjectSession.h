#pragma once

#include <cstddef>
#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include "Core/Error.h"
#include "Domain/Project.h"
#include "Domain/ProjectHistory.h"
#include "Domain/ProjectSerializer.h"

namespace Tiles
{
	// Owns the active project and the recent-projects history, and performs all
	// project persistence (create/save/save-as/load) via ProjectSerializer. It
	// holds no editing, undo, or camera state; the owner resets those around a
	// create or load.
	class ProjectSession
	{
	public:
		// Starts with a fresh, unsaved "Untitled" project.
		ProjectSession();

		// The active project (never null in the current single-project model).
		[[nodiscard]] std::shared_ptr<Project> GetProject() const { return m_Project; }

		// True while a project is loaded.
		[[nodiscard]] bool HasProject() const { return m_Project != nullptr; }

		// True while a project is loaded and has unsaved changes.
		[[nodiscard]] bool IsDirty() const { return m_Project && m_Project->HasUnsavedChanges(); }

		// Project name, suffixed with "(Unsaved)" while it has no file path.
		[[nodiscard]] std::string GetDisplayName() const;

		// Replaces the project with a fresh, empty one.
		void Create(const std::string& name);

		// Closes the active project, leaving the session with no project loaded.
		void Close() { m_Project.reset(); }

		// Serializes to the project's existing file path.
		[[nodiscard]] std::expected<void, Error> Save();

		// Serializes to path and adopts it as the project's file path.
		[[nodiscard]] std::expected<void, Error> SaveAs(const std::filesystem::path& path);

		// Loads path into the active project, recording it in the history.
		// A missing file is dropped from the history and reported as a failure.
		[[nodiscard]] std::expected<void, Error> Load(const std::filesystem::path& path);

		// The number of tracked recent projects.
		[[nodiscard]] size_t GetRecentCount() const { return m_History.GetCount(); }

		// True while the recent-projects list is non-empty.
		[[nodiscard]] bool HasRecent() const { return !m_History.IsEmpty(); }

		// The recent-projects entry at index.
		[[nodiscard]] const ProjectHistoryEntry& GetRecent(size_t index) const { return m_History.GetEntry(index); }

		// Clears the recent-projects list.
		void ClearRecent() { m_History.Clear(); }

	private:
		std::shared_ptr<Project> m_Project;
		ProjectHistory m_History;
	};
}
