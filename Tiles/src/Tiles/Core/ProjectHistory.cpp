#include "ProjectHistory.h"
#include "Lumina/Core/Log.h"
#include <fstream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#elif __APPLE__
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace Tiles
{
    nlohmann::json ProjectHistoryEntry::ToJSON() const
    {
        nlohmann::json json;
        json["filePath"] = filePath.string();
        json["displayName"] = displayName;
        json["lastAccessed"] = std::chrono::duration_cast<std::chrono::seconds>(
            lastAccessed.time_since_epoch()).count();
        return json;
    }

    ProjectHistoryEntry ProjectHistoryEntry::FromJSON(const nlohmann::json& json)
    {
        ProjectHistoryEntry entry;
        entry.filePath = json.value("filePath", "");
        entry.displayName = json.value("displayName", "");

        auto timestamp = json.value("lastAccessed", 0);
        entry.lastAccessed = std::chrono::system_clock::from_time_t(timestamp);

        return entry;
    }

    ProjectHistory::ProjectHistory()
    {
        Load();
    }

    ProjectHistory::~ProjectHistory()
    {
        if (m_HasChanges)
        {
            Save();
        }
    }

    void ProjectHistory::AddProject(const std::filesystem::path& filePath, const std::string& displayName)
    {
        if (filePath.empty() || displayName.empty())
            return;

        RemoveProject(filePath);

        ProjectHistoryEntry newEntry;
        newEntry.filePath = filePath;
        newEntry.displayName = displayName;
        newEntry.lastAccessed = std::chrono::system_clock::now();

        m_History.insert(m_History.begin(), newEntry);

        if (m_History.size() > MAX_HISTORY_SIZE)
        {
            m_History.resize(MAX_HISTORY_SIZE);
        }

        m_HasChanges = true;
        LUMINA_LOG_INFO("ProjectHistory::AddProject: Added '{}' to project history", displayName);
    }

    void ProjectHistory::RemoveProject(const std::filesystem::path& filePath)
    {
        auto it = std::find_if(m_History.begin(), m_History.end(),
            [&filePath](const ProjectHistoryEntry& entry) {
                return entry.filePath == filePath;
            });

        if (it != m_History.end())
        {
            m_History.erase(it);
            m_HasChanges = true;
        }
    }

    void ProjectHistory::Clear()
    {
        if (!m_History.empty())
        {
            m_History.clear();
            m_HasChanges = true;
            LUMINA_LOG_INFO("ProjectHistory::Clear: Cleared all project history");
        }
    }

    std::filesystem::path ProjectHistory::GetAppDataPath() const
    {
        std::filesystem::path appDataPath;

#ifdef _WIN32
        PWSTR path;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path)))
        {
            appDataPath = path;
            CoTaskMemFree(path);
            appDataPath /= "TilesEditor";
        }
#elif __APPLE__
        const char* home = getenv("HOME");
        if (home)
        {
            appDataPath = home;
            appDataPath /= "Library/Application Support/TilesEditor";
        }
#else
        const char* home = getenv("HOME");
        if (home)
        {
            appDataPath = home;
            appDataPath /= ".config/TilesEditor";
        }
#endif

        if (!std::filesystem::exists(appDataPath))
        {
            std::filesystem::create_directories(appDataPath);
        }

        return appDataPath / "project_history.json";
    }

    void ProjectHistory::RemoveInvalidEntries()
    {
        auto it = std::remove_if(m_History.begin(), m_History.end(),
            [](const ProjectHistoryEntry& entry) {
                return !std::filesystem::exists(entry.filePath);
            });

        if (it != m_History.end())
        {
            size_t removedCount = std::distance(it, m_History.end());
            m_History.erase(it, m_History.end());
            m_HasChanges = true;
            LUMINA_LOG_INFO("ProjectHistory::RemoveInvalidEntries: Removed {} invalid entries", removedCount);
        }
    }

    void ProjectHistory::Load()
    {
        std::filesystem::path configPath = GetAppDataPath();

        if (!std::filesystem::exists(configPath))
        {
            return;
        }

        try
        {
            std::ifstream file(configPath);
            if (!file.is_open())
            {
                return;
            }

            nlohmann::json json;
            file >> json;

            if (json.contains("projectHistory") && json["projectHistory"].is_array())
            {
                for (const auto& entryJson : json["projectHistory"])
                {
                    try
                    {
                        ProjectHistoryEntry entry = ProjectHistoryEntry::FromJSON(entryJson);
                        m_History.push_back(entry);
                    }
                    catch (const std::exception& e)
                    {
                        LUMINA_LOG_INFO("ProjectHistory::Load: Failed to parse entry: {}", e.what());
                    }
                }
            }

            RemoveInvalidEntries();
            LUMINA_LOG_INFO("ProjectHistory::Load: Loaded {} project history entries", m_History.size());
        }
        catch (const std::exception& e)
        {
            LUMINA_LOG_INFO("ProjectHistory::Load: Failed to load project history: {}", e.what());
            m_History.clear();
        }
    }

    void ProjectHistory::Save()
    {
        if (!m_HasChanges)
            return;

        std::filesystem::path configPath = GetAppDataPath();

        try
        {
            nlohmann::json json;
            json["projectHistory"] = nlohmann::json::array();

            for (const auto& entry : m_History)
            {
                json["projectHistory"].push_back(entry.ToJSON());
            }

            std::ofstream file(configPath);
            if (!file.is_open())
            {
                LUMINA_LOG_INFO("ProjectHistory::Save: Could not open file for writing");
                return;
            }

            file << json.dump(4);
            m_HasChanges = false;
            LUMINA_LOG_INFO("ProjectHistory::Save: Saved {} project history entries", m_History.size());
        }
        catch (const std::exception& e)
        {
            LUMINA_LOG_INFO("ProjectHistory::Save: Failed to save project history: {}", e.what());
        }
    }
}