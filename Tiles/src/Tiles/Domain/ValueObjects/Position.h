#pragma once

#include <cstdint>
#include <functional>

namespace Tiles::Domain
{
    struct Position
    {
        uint32_t X = 0;
        uint32_t Y = 0;

        Position() = default;
        Position(uint32_t x, uint32_t y) : X(x), Y(y) {}

        bool operator==(const Position& other) const
        {
            return X == other.X && Y == other.Y;
        }

        bool operator!=(const Position& other) const
        {
            return !(*this == other);
        }
    };
}

// Hash function for Position (allows use in unordered containers)
namespace std
{
    template<>
    struct hash<Tiles::Domain::Position>
    {
        size_t operator()(const Tiles::Domain::Position& pos) const
        {
            return hash<uint32_t>()(pos.X) ^ (hash<uint32_t>()(pos.Y) << 1);
        }
    };
}
