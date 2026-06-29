#pragma once

#include "TileGrid.h"

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <filesystem>
#include <optional>

namespace Tiles::Domain
{
    // Aggregate root: A complete tile map project
    // Contains layers, metadata, and asset references
    class TileProject
    {
    public:
        using TimePoint = std::chrono::steady_clock::time_point;

        TileProject(uint32_t width, uint32_t height, const std::string& name = "Untitled");
        ~TileProject() = default;

        // Project metadata
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name);

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        void Resize(uint32_t width, uint32_t height);

        // File path management
        const std::filesystem::path& GetFilePath() const { return m_FilePath; }
        void SetFilePath(const std::filesystem::path& path) { m_FilePath = path; }
        bool HasFilePath() const { return !m_FilePath.empty(); }
        bool IsNew() const { return !HasFilePath(); }

        // Modification tracking
        bool HasUnsavedChanges() const { return m_IsDirty; }
        void MarkAsModified();
        void MarkAsSaved();

        // Timestamps
        TimePoint GetCreatedTime() const { return m_CreatedTime; }
        TimePoint GetLastModifiedTime() const { return m_LastModifiedTime; }
        TimePoint GetLastSavedTime() const { return m_LastSavedTime; }

        // Layer management
        size_t GetLayerCount() const { return m_Layers.size(); }
        TileGrid& GetLayer(size_t index);
        const TileGrid& GetLayer(size_t index) const;

        size_t AddLayer(const std::string& name = "New Layer");
        void RemoveLayer(size_t index);
        void MoveLayerUp(size_t index);
        void MoveLayerDown(size_t index);
        void SwapLayers(size_t index1, size_t index2);
        void InsertLayer(size_t index, const std::string& name = "New Layer");
        void DuplicateLayer(size_t index);

        // Atlas references (just IDs, actual textures are in infrastructure)
        const std::vector<std::string>& GetAtlasReferences() const { return m_AtlasReferences; }
        void AddAtlasReference(const std::string& atlasPath);
        void RemoveAtlasReference(size_t index);
        void ClearAtlasReferences();

    private:
        std::string m_Name;
        uint32_t m_Width;
        uint32_t m_Height;
        std::filesystem::path m_FilePath;

        bool m_IsDirty = false;
        TimePoint m_CreatedTime;
        TimePoint m_LastModifiedTime;
        TimePoint m_LastSavedTime;

        std::vector<std::unique_ptr<TileGrid>> m_Layers;
        std::vector<std::string> m_AtlasReferences;
    };
}
