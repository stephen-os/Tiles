#pragma once

#include <string>
#include <vector>

#include "Tile.h"

struct TileLayer
{
    std::vector<std::vector<Tile>> m_Tiles;
    std::string m_Name;
};