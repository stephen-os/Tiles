#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <filesystem>

#include "json.hpp"

#include "Base.h"
#include "Tile.h"
#include "TileLayer.h"
#include "LayerStack.h"

#include "Lumina/Lumina.h"

namespace Tiles
{
    class Project
    {
    public:
        nlohmann::json ToJSON() const;
        static Ref<Project> FromJSON(const nlohmann::json& json);

        Project(uint32_t width, uint32_t height, const std::string& name = "Untitled Project");
        ~Project() = default;

        const std::string& GetProjectName() const { return m_ProjectName; }
        const std::filesystem::path& GetFilePath() const { return m_FilePath; }
        auto GetLastAccessed() const { return m_LastAccessed; }
        auto GetLastSaved() const { return m_LastSaved; }

        void SetProjectName(const std::string& name);
        void SetFilePath(const std::filesystem::path& path) { m_FilePath = path; }
        void UpdateLastAccessed() { m_LastAccessed = std::chrono::steady_clock::now(); }
        void UpdateLastSaved() { m_LastSaved = std::chrono::steady_clock::now(); }

        void MarkAsModified() { m_HasUnsavedChanges = true; }
        void MarkAsSaved() { m_HasUnsavedChanges = false; UpdateLastSaved(); }
        bool IsNew() const { return m_FilePath.empty(); }
        bool HasUnsavedChanges() const { return m_HasUnsavedChanges; }

        std::vector<Ref<Lumina::TextureAtlas>>& GetTextureAtlases() { return m_TextureAtlases; }
        const std::vector<Ref<Lumina::TextureAtlas>>& GetTextureAtlases() const { return m_TextureAtlases; }

        LayerStack& GetLayerStack() { return m_LayerStack; }
        const LayerStack& GetLayerStack() const { return m_LayerStack; }

        void AddTextureAtlas(Ref<Lumina::TextureAtlas> atlas);
        Ref<Lumina::TextureAtlas> GetTextureAtlas(size_t index); 
        void RemoveTextureAtlas(size_t index);
        void ClearTextureAtlases();
        size_t GetTextureAtlasCount() const { return m_TextureAtlases.size(); }

    private:
        std::string m_ProjectName = "New Project";                              // Display name of the project shown in UI
        std::filesystem::path m_FilePath = "";                                  // File path where project is saved (empty for new projects)
        std::chrono::steady_clock::time_point m_LastAccessed;                   // Timestamp when project was last opened/accessed
        std::chrono::steady_clock::time_point m_LastSaved;                      // Timestamp when project was last saved to disk
        bool m_HasUnsavedChanges = false;                                       // Flag indicating unsaved modifications exist
        std::vector<Ref<Lumina::TextureAtlas>> m_TextureAtlases;                // Collection of texture atlases for sprites/tiles
        LayerStack m_LayerStack;                                                // Stack of tile layers containing the actual tile data
    };
}