#pragma once
#include <string>
#include <unordered_map>
#include <optional>
#include <cstdint>

#include <glm/glm.hpp>

#include "Domain/Tile.h"
#include "json.hpp"

namespace Tiles
{
    enum class RenderGroup : int32_t
    {
        Disabled = -1,      // Layer is not rendered
        Background = 0,     // Default background layer
        Midground = 1,      // Middle layer for objects
        Foreground = 2,     // Front layer for UI/overlays
        Debug = 99          // Debug/temporary rendering
    };

    namespace TileLayerUtils
    {
        static const char* GetRenderGroupName(RenderGroup renderGroup)
        {
            switch (renderGroup)
            {
            case RenderGroup::Disabled: return "Disabled";
            case RenderGroup::Background: return "Background";
            case RenderGroup::Midground: return "Midground";
            case RenderGroup::Foreground: return "Foreground";
            case RenderGroup::Debug: return "Debug";
            default: return "Unknown";
            }
        }

        static std::vector<RenderGroup> GetAllRenderGroups()
        {
            return {
                RenderGroup::Disabled,
                RenderGroup::Background,
                RenderGroup::Midground,
                RenderGroup::Foreground,
                RenderGroup::Debug
            };
        }

        static std::vector<const char*> GetAllRenderGroupNames()
        {
            std::vector<const char*> names;
            for (const auto& group : GetAllRenderGroups())
            {
                names.push_back(GetRenderGroupName(group));
            }
            return names;
        }

        static std::vector<int32_t> GetAllRenderGroupValues()
        {
            std::vector<int32_t> values;
            for (const auto& group : GetAllRenderGroups())
            {
                values.push_back(static_cast<int32_t>(group));
            }
            return values;
        }

        static size_t GetRenderGroupCount()
        {
            return GetAllRenderGroups().size();
        }
    }

    /// Hash for signed integer tile coordinates used as sparse-map keys.
    struct TileCoordHash
    {
        size_t operator()(const glm::ivec2& c) const noexcept
        {
            // Pack the two 32-bit coords into one 64-bit key, then hash.
            uint64_t key = (static_cast<uint64_t>(static_cast<uint32_t>(c.x)) << 32)
                | static_cast<uint32_t>(c.y);
            return std::hash<uint64_t>()(key);
        }
    };

    /// A sparse grid of tiles with a name, visibility, and render group. Only
    /// painted cells are stored, addressed by signed (x, y), so the board is
    /// unbounded in every direction and empty cells cost nothing.
    class TileLayer
    {
    public:
        using TileMap = std::unordered_map<glm::ivec2, Tile, TileCoordHash>;

        TileLayer() = default;
        ~TileLayer() = default;

        /// Removes every painted tile.
        void Clear();

        /// The tile at (x, y), or a shared empty (unpainted) tile if none exists.
        const Tile& GetTile(int x, int y) const;
        bool HasTile(int x, int y) const;

        /// Paints (x, y). An unpainted tile erases the cell, keeping the map minimal.
        void SetTile(int x, int y, const Tile& tile);
        /// Clears a single cell.
        void EraseTile(int x, int y);

        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name);

        bool GetVisibility() const { return m_Visible; }
        void SetVisibility(bool visible) { m_Visible = visible; }

        RenderGroup GetRenderGroup() const { return m_RenderGroup; }
        void SetRenderGroup(RenderGroup group) { m_RenderGroup = group; }
        void DisableRendering() { m_RenderGroup = RenderGroup::Disabled; }
        bool IsRenderingEnabled() const { return m_RenderGroup != RenderGroup::Disabled; }

        size_t GetTileCount() const { return m_Tiles.size(); }
        bool IsEmpty() const { return m_Tiles.empty(); }

        // Iteration yields (const glm::ivec2 coord, Tile) pairs for painted cells only.
        TileMap::iterator begin() { return m_Tiles.begin(); }
        TileMap::iterator end() { return m_Tiles.end(); }
        TileMap::const_iterator begin() const { return m_Tiles.begin(); }
        TileMap::const_iterator end() const { return m_Tiles.end(); }

        /// Inclusive bounding box of painted tiles as (minX, minY, maxX, maxY),
        /// or nullopt when the layer is empty.
        std::optional<glm::ivec4> GetBounds() const;

        nlohmann::json ToJSON() const;
        static TileLayer FromJSON(const nlohmann::json& jsonLayer);

    private:
        std::string m_Name = "New Layer";                       // Display name of the layer
        bool m_Visible = true;                                  // Whether layer is visible in editor/game
        RenderGroup m_RenderGroup = RenderGroup::Background;    // Rendering order group
        TileMap m_Tiles;                                        // Sparse map of painted tiles by coordinate
    };
}
