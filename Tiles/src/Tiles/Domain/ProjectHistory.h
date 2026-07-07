#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <chrono>

#include "json.hpp"

namespace Tiles
{
    struct ProjectHistoryEntry
    {
        std::filesystem::path filePath;
        std::string displayName;
        std::chrono::system_clock::time_point lastAccessed;

        nlohmann::json ToJSON() const;
        static ProjectHistoryEntry FromJSON(const nlohmann::json& json);
    };

    // Most-recently-used list of opened/saved projects, persisted to a JSON
    // file in the platform's app-data directory. Loads on construction and
    // saves on destruction when dirty.
    class ProjectHistory
    {
    public:
        static constexpr size_t MAX_HISTORY_SIZE = 10;

        ProjectHistory();
        ~ProjectHistory();

        // Records a project as most-recent, moving it to the front if already
        // present and trimming the list to MAX_HISTORY_SIZE. Empty paths/names
        // are ignored.
        void AddProject(const std::filesystem::path& filePath, const std::string& displayName);
        void Clear();

        size_t GetCount() const { return m_History.size(); }
        bool IsEmpty() const { return m_History.empty(); }
        const ProjectHistoryEntry& GetEntry(size_t index) const { return m_History[index]; }

        void Load();
        void Save();

    private:
        // Full path to the history JSON file, creating its directory if needed.
        std::filesystem::path GetAppDataPath() const;
        // Drops entries whose file no longer exists on disk.
        void RemoveInvalidEntries();
        void RemoveProject(const std::filesystem::path& filePath);

        std::vector<ProjectHistoryEntry> m_History;
        bool m_HasChanges = false;

        friend class ProjectSession;
    };
}