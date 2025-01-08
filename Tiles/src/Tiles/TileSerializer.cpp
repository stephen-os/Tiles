#include "TileSerializer.h"

#include <fstream>
#include <iostream>

void TileSerializer::Serialize(const std::vector<LayerData>& layers, const std::string& path)
{
    if (!std::filesystem::exists(path))
    {
        std::cout << "File not found: " << path << ". Creating a new file." << std::endl;
        std::ofstream newFile(path);
        if (!newFile.is_open())
        {
            std::cerr << "Failed to create file: " << path << std::endl;
            return;
        }
        newFile.close();
    }

    nlohmann::json jsonLayers;

    for (const auto& layer : layers)
    {
        nlohmann::json jsonLayer;
        jsonLayer["name"] = layer.Name;

        for (const auto& row : layer.Layer)
        {
            nlohmann::json jsonRow;
            for (const auto& tile : row)
            {
                nlohmann::json jsonTile;
                jsonTile["color"] = { tile.Color.r, tile.Color.g, tile.Color.b, tile.Color.a };
                jsonTile["opacity"] = tile.Opacity;

                jsonTile["use_texture"] = tile.UseTexture;
                jsonTile["texture_index"] = tile.TextureIndex;

                jsonRow.push_back(jsonTile);
            }
            jsonLayer["tiles"].push_back(jsonRow);
        }
        jsonLayers.push_back(jsonLayer);
    }

    std::ofstream file(path);
    if (file.is_open())
    {
        file << jsonLayers.dump(4);
        file.close();
    }
    else
    {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
    }
}

std::vector<LayerData> TileSerializer::Deserialize(const std::string& path)
{
    std::vector<LayerData> layers;
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file for reading: " << path << std::endl;
        return layers;
    }

    nlohmann::json jsonLayers;
    file >> jsonLayers;

    for (const auto& jsonLayer : jsonLayers)
    {
        LayerData layer;
        layer.Name = jsonLayer.at("name").get<std::string>();

        for (const auto& jsonRow : jsonLayer.at("tiles"))
        {
            std::vector<TileData> row;
            for (const auto& jsonTile : jsonRow)
            {
                TileData tile;
                auto color = jsonTile.at("color").get<std::vector<float>>();
                tile.Color = glm::vec4(color[0], color[1], color[2], color[3]);
                tile.UseTexture = jsonTile.at("use_texture").get<bool>();
                tile.Opacity = jsonTile.at("opacity").get<float>();
                tile.TextureIndex = jsonTile.at("texture_index").get<int>();

                row.push_back(tile);
            }
            layer.Layer.push_back(row);
        }
        layers.push_back(layer);
    }

    return layers;
}
