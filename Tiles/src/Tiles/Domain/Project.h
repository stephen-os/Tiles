#pragma once
#include <cstdint>

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <filesystem>

#include "json.hpp"

#include "Domain/Tile.h"
#include "Domain/TileLayer.h"
#include "Domain/LayerStack.h"

#include "../Graphics/TextureAtlas.h"

namespace Tiles
{
    /// A user-defined rectangular export region in tile coordinates. When enabled
    /// it defines what the exporter renders, and is drawn as a guide in the
    /// viewport; otherwise export falls back to the painted content's bounds.
    struct ExportRegion
    {
        glm::ivec2 Min{ 0, 0 };       // bottom-left tile (inclusive)
        glm::ivec2 Size{ 16, 16 };    // size in tiles
        bool Enabled = false;
    };

    /// A document: the layer stack, its texture atlases, and file/dirty
    /// bookkeeping. A project with no file path is considered "new" (unsaved).
    class Project
    {
    public:
        nlohmann::json ToJSON() const;
        /// Reconstructs a project from JSON.
        /// @return Null if the required layer-stack field is missing.
        static std::shared_ptr<Project> FromJSON(const nlohmann::json& json);

        explicit Project(const std::string& name = "Untitled Project");
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

        std::vector<std::shared_ptr<Tiles::TextureAtlas>>& GetTextureAtlases() { return m_TextureAtlases; }
        const std::vector<std::shared_ptr<Tiles::TextureAtlas>>& GetTextureAtlases() const { return m_TextureAtlases; }

        LayerStack& GetLayerStack() { return m_LayerStack; }
        const LayerStack& GetLayerStack() const { return m_LayerStack; }

        ExportRegion& GetExportRegion() { return m_ExportRegion; }
        const ExportRegion& GetExportRegion() const { return m_ExportRegion; }

        void AddTextureAtlas(std::shared_ptr<Tiles::TextureAtlas> atlas);
        std::shared_ptr<Tiles::TextureAtlas> GetTextureAtlas(size_t index);
        void RemoveTextureAtlas(size_t index);
        void ClearTextureAtlases();
        size_t GetTextureAtlasCount() const { return m_TextureAtlases.size(); }

    private:
        std::string m_ProjectName = "New Project";                              // Display name of the project shown in UI
        std::filesystem::path m_FilePath = "";                                  // File path where project is saved (empty for new projects)
        std::chrono::steady_clock::time_point m_LastAccessed;                   // Timestamp when project was last opened/accessed
        std::chrono::steady_clock::time_point m_LastSaved;                      // Timestamp when project was last saved to disk
        bool m_HasUnsavedChanges = false;                                       // Flag indicating unsaved modifications exist
        std::vector<std::shared_ptr<Tiles::TextureAtlas>> m_TextureAtlases;                // Collection of texture atlases for sprites/tiles
        LayerStack m_LayerStack;                                                // Stack of tile layers containing the actual tile data
        ExportRegion m_ExportRegion;                                            // User-defined export/canvas region (tile space)
    };
}