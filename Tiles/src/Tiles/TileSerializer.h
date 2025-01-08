#include <string>
#include <vector>
#include "TileLayer.h"
#include "json.hpp"

class TileSerializer
{
public:
    static void Serialize(const std::vector<TileLayer>& layers, const std::string& filePath = "res/maps/tiles.map");
    static std::vector<TileLayer> Deserialize(const std::string& filePath = "res/maps/tiles.map");
};
