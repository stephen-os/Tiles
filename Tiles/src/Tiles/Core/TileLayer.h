#pragma once
#include <string>
#include <vector>

#include "Tile.h"
#include "json.hpp"

#include "Constants.h" 

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
    
    class TileLayer
    {
    public:
        TileLayer() = default;
        TileLayer(uint32_t width, uint32_t height);
        ~TileLayer() = default;

        void Clear();
        void Resize(uint32_t width, uint32_t height);

        // Tile access
        const Tile& GetTile(size_t x, size_t y) const;
        Tile& GetTile(size_t x, size_t y);

        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name);

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        void SetWidth(uint32_t width);
        void SetHeight(uint32_t height);

        bool GetVisibility() const { return m_Visible; }
        void SetVisibility(bool visible) { m_Visible = visible; }

        RenderGroup GetRenderGroup() const { return m_RenderGroup; }
        void SetRenderGroup(RenderGroup group) { m_RenderGroup = group; }
        void DisableRendering() { m_RenderGroup = RenderGroup::Disabled; }
        bool IsRenderingEnabled() const { return m_RenderGroup != RenderGroup::Disabled; }

        size_t GetTileCount() const { return m_Tiles.size(); }
        bool IsEmpty() const { return m_Tiles.empty(); }
        bool IsValidPosition(size_t x, size_t y) const { return x < m_Width && y < m_Height; }

        auto begin() { return m_Tiles.begin(); }
        auto end() { return m_Tiles.end(); }
        auto begin() const { return m_Tiles.begin(); }
        auto end() const { return m_Tiles.end(); }

        nlohmann::json ToJSON() const;
        static TileLayer FromJSON(const nlohmann::json& jsonLayer);

    private:
        void ResizeInternal(uint32_t width, uint32_t height);

    private:
        std::string m_Name = "New Layer";                       // Display name of the layer
        uint32_t m_Width = 0;                                   // Width in tiles
        uint32_t m_Height = 0;                                  // Height in tiles
        bool m_Visible = true;                                  // Whether layer is visible in editor/game
        RenderGroup m_RenderGroup = RenderGroup::Background;    // Rendering order group
        std::vector<Tile> m_Tiles;                              // Flat array of tiles (row-major order)
    };
}