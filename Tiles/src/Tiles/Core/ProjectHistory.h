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

    class ProjectHistory
    {
    public:
        static constexpr size_t MAX_HISTORY_SIZE = 10;

        ProjectHistory();
        ~ProjectHistory();

        void AddProject(const std::filesystem::path& filePath, const std::string& displayName);
        void Clear();

        size_t GetCount() const { return m_History.size(); }
        bool IsEmpty() const { return m_History.empty(); }
        const ProjectHistoryEntry& GetEntry(size_t index) const { return m_History[index]; }

        void Load();
        void Save();

    private:
        std::filesystem::path GetAppDataPath() const;
        void RemoveInvalidEntries();
        void RemoveProject(const std::filesystem::path& filePath);

        std::vector<ProjectHistoryEntry> m_History;
        bool m_HasChanges = false;

        friend class Context;
    };
}