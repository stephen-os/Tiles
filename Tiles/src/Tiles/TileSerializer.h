#pragma once

#include <string>
#include <vector>

#include "TileLayer.h"

#include "json.hpp"

class TileSerializer
{
public:
    static void Serialize(const std::vector<LayerData>& layers, const std::string& path);
    static std::vector<LayerData> Deserialize(const std::string& path);
};
