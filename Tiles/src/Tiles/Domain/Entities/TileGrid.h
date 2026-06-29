#pragma once

#include "../ValueObjects/Position.h"
#include "../ValueObjects/TileData.h"

#include <vector>
#include <string>
#include <optional>
#include <functional>

namespace Tiles::Domain
{
    // Core domain entity: A 2D grid of tiles
    // Contains no rendering logic - pure domain model
    class TileGrid
    {
    public:
        TileGrid(uint32_t width, uint32_t height, const std::string& name = "Layer");
        ~TileGrid() = default;

        // Dimensions
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        void Resize(uint32_t newWidth, uint32_t newHeight);

        // Name
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

        // Visibility
        bool IsVisible() const { return m_Visible; }
        void SetVisible(bool visible) { m_Visible = visible; }

        // Tile access
        const TileData& GetTile(uint32_t x, uint32_t y) const;
        const TileData& GetTile(const Position& pos) const;
        void SetTile(uint32_t x, uint32_t y, const TileData& tile);
        void SetTile(const Position& pos, const TileData& tile);
        void ClearTile(uint32_t x, uint32_t y);
        void ClearTile(const Position& pos);

        // Bulk operations
        void Clear();
        void Fill(const TileData& tile);

        // Bounds checking
        bool IsInBounds(uint32_t x, uint32_t y) const;
        bool IsInBounds(const Position& pos) const;

        // Iteration support
        void ForEachTile(const std::function<void(uint32_t x, uint32_t y, const TileData& tile)>& callback) const;
        void ForEachNonEmptyTile(const std::function<void(uint32_t x, uint32_t y, const TileData& tile)>& callback) const;

        // Raw data access (for serialization)
        const std::vector<TileData>& GetRawData() const { return m_Tiles; }
        void SetRawData(const std::vector<TileData>& data);

    private:
        size_t GetIndex(uint32_t x, uint32_t y) const;

        uint32_t m_Width;
        uint32_t m_Height;
        std::string m_Name;
        bool m_Visible = true;
        std::vector<TileData> m_Tiles;

        static TileData s_EmptyTile; // Returned for out-of-bounds access
    };
}
