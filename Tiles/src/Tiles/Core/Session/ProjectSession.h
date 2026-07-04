#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>

#include "Domain/Project.h"
#include "Domain/ProjectHistory.h"
#include "Domain/ProjectSerializer.h"

namespace Tiles
{
    /// Owns the active project and the recent-projects history, and performs all
    /// project persistence (create/save/save-as/load) via ProjectSerializer. It
    /// holds no editing, undo, or camera state; the owner resets those around a
    /// create or load.
    class ProjectSession
    {
    public:
        ProjectSession();

        std::shared_ptr<Project> GetProject() const { return m_Project; }
        bool HasProject() const { return m_Project != nullptr; }
        bool IsDirty() const { return m_Project->HasUnsavedChanges(); }
        /// Project name, suffixed with "(Unsaved)" while it has no file path.
        std::string GetDisplayName() const;

        /// Replaces the project with a fresh, empty one.
        void Create(const std::string& name);
        /// Serializes to the project's existing file path.
        ProjectResult Save();
        /// Serializes to path and adopts it as the project's file path.
        ProjectResult SaveAs(const std::filesystem::path& path);
        /// Loads path into the active project, recording it in the history.
        /// A missing file is dropped from the history and reported as a failure.
        ProjectResult Load(const std::filesystem::path& path);

        size_t GetRecentCount() const { return m_History.GetCount(); }
        bool HasRecent() const { return !m_History.IsEmpty(); }
        const ProjectHistoryEntry& GetRecent(size_t index) const { return m_History.GetEntry(index); }
        void ClearRecent() { m_History.Clear(); }

    private:
        std::shared_ptr<Project> m_Project;
        ProjectHistory m_History;
    };
}
