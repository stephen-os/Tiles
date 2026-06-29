#pragma once

#include <cstdint>

namespace Tiles::Domain
{
    // Immutable value object representing tile data
    // No rendering logic - pure data
    struct TileData
    {
        uint32_t AtlasIndex = 0;      // Which texture atlas this tile uses
        uint32_t TileIndex = 0;       // Index within the atlas
        float Rotation = 0.0f;        // Rotation in degrees
        bool FlipX = false;           // Horizontal flip
        bool FlipY = false;           // Vertical flip

        TileData() = default;

        TileData(uint32_t atlasIndex, uint32_t tileIndex, float rotation = 0.0f,
                 bool flipX = false, bool flipY = false)
            : AtlasIndex(atlasIndex)
            , TileIndex(tileIndex)
            , Rotation(rotation)
            , FlipX(flipX)
            , FlipY(flipY)
        {}

        bool operator==(const TileData& other) const
        {
            return AtlasIndex == other.AtlasIndex &&
                   TileIndex == other.TileIndex &&
                   Rotation == other.Rotation &&
                   FlipX == other.FlipX &&
                   FlipY == other.FlipY;
        }

        bool operator!=(const TileData& other) const
        {
            return !(*this == other);
        }

        bool IsEmpty() const
        {
            return AtlasIndex == 0 && TileIndex == 0;
        }

        static TileData Empty()
        {
            return TileData{};
        }
    };
}
