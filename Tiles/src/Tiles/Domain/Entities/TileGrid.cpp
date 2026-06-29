#include "TileGrid.h"

#include <algorithm>
#include <stdexcept>
#include <limits>

namespace Tiles::Domain
{
    // Security limits to prevent overflow attacks
    namespace {
        constexpr uint32_t MaxGridDimension = 10000;
        constexpr size_t MaxTileCount = 100'000'000; // 100 million tiles max

        [[nodiscard]] size_t SafeMultiply(uint32_t a, uint32_t b) {
            // Use size_t to prevent overflow during multiplication
            size_t result = static_cast<size_t>(a) * static_cast<size_t>(b);

            // Overflow check: if result/a != b, overflow occurred (when a != 0)
            if (a != 0 && result / a != b) {
                throw std::overflow_error("Grid dimension overflow");
            }
            return result;
        }
    }

    TileData TileGrid::s_EmptyTile = TileData::Empty();

    TileGrid::TileGrid(uint32_t width, uint32_t height, const std::string& name)
        : m_Width(std::min(width, MaxGridDimension))
        , m_Height(std::min(height, MaxGridDimension))
        , m_Name(name)
    {
        // Ensure dimensions are at least 1
        if (m_Width == 0) m_Width = 1;
        if (m_Height == 0) m_Height = 1;

        size_t tileCount = SafeMultiply(m_Width, m_Height);
        if (tileCount > MaxTileCount) {
            throw std::length_error("Grid too large: " + std::to_string(tileCount) + " tiles exceeds limit of " + std::to_string(MaxTileCount));
        }
        m_Tiles.resize(tileCount);
    }

    void TileGrid::Resize(uint32_t newWidth, uint32_t newHeight)
    {
        // Clamp and validate dimensions
        newWidth = std::clamp(newWidth, 1u, MaxGridDimension);
        newHeight = std::clamp(newHeight, 1u, MaxGridDimension);

        if (newWidth == m_Width && newHeight == m_Height)
            return;

        size_t newTileCount = SafeMultiply(newWidth, newHeight);
        if (newTileCount > MaxTileCount) {
            throw std::length_error("Resize would exceed maximum tile count");
        }

        std::vector<TileData> newTiles(newTileCount);

        // Copy existing tiles that fit in new dimensions
        uint32_t copyWidth = std::min(m_Width, newWidth);
        uint32_t copyHeight = std::min(m_Height, newHeight);

        for (uint32_t y = 0; y < copyHeight; ++y)
        {
            for (uint32_t x = 0; x < copyWidth; ++x)
            {
                size_t oldIndex = static_cast<size_t>(y) * m_Width + x;
                size_t newIndex = static_cast<size_t>(y) * newWidth + x;
                newTiles[newIndex] = m_Tiles[oldIndex];
            }
        }

        m_Width = newWidth;
        m_Height = newHeight;
        m_Tiles = std::move(newTiles);
    }

    const TileData& TileGrid::GetTile(uint32_t x, uint32_t y) const
    {
        if (!IsInBounds(x, y))
            return s_EmptyTile;

        return m_Tiles[GetIndex(x, y)];
    }

    const TileData& TileGrid::GetTile(const Position& pos) const
    {
        return GetTile(pos.X, pos.Y);
    }

    void TileGrid::SetTile(uint32_t x, uint32_t y, const TileData& tile)
    {
        if (!IsInBounds(x, y))
            return;

        m_Tiles[GetIndex(x, y)] = tile;
    }

    void TileGrid::SetTile(const Position& pos, const TileData& tile)
    {
        SetTile(pos.X, pos.Y, tile);
    }

    void TileGrid::ClearTile(uint32_t x, uint32_t y)
    {
        SetTile(x, y, TileData::Empty());
    }

    void TileGrid::ClearTile(const Position& pos)
    {
        ClearTile(pos.X, pos.Y);
    }

    void TileGrid::Clear()
    {
        std::fill(m_Tiles.begin(), m_Tiles.end(), TileData::Empty());
    }

    void TileGrid::Fill(const TileData& tile)
    {
        std::fill(m_Tiles.begin(), m_Tiles.end(), tile);
    }

    bool TileGrid::IsInBounds(uint32_t x, uint32_t y) const
    {
        return x < m_Width && y < m_Height;
    }

    bool TileGrid::IsInBounds(const Position& pos) const
    {
        return IsInBounds(pos.X, pos.Y);
    }

    void TileGrid::ForEachTile(const std::function<void(uint32_t, uint32_t, const TileData&)>& callback) const
    {
        for (uint32_t y = 0; y < m_Height; ++y)
        {
            for (uint32_t x = 0; x < m_Width; ++x)
            {
                callback(x, y, m_Tiles[GetIndex(x, y)]);
            }
        }
    }

    void TileGrid::ForEachNonEmptyTile(const std::function<void(uint32_t, uint32_t, const TileData&)>& callback) const
    {
        for (uint32_t y = 0; y < m_Height; ++y)
        {
            for (uint32_t x = 0; x < m_Width; ++x)
            {
                const auto& tile = m_Tiles[GetIndex(x, y)];
                if (!tile.IsEmpty())
                {
                    callback(x, y, tile);
                }
            }
        }
    }

    void TileGrid::SetRawData(const std::vector<TileData>& data)
    {
        if (data.size() != static_cast<size_t>(m_Width) * m_Height)
        {
            throw std::invalid_argument("Data size does not match grid dimensions");
        }
        m_Tiles = data;
    }

    size_t TileGrid::GetIndex(uint32_t x, uint32_t y) const
    {
        return static_cast<size_t>(y) * m_Width + x;
    }
}
