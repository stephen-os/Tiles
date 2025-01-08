#include "TileSerializer.h"
#include <fstream>
#include <iostream>

void TileSerializer::Serialize(const std::vector<TileLayer>& layers, const std::string& filePath)
{
    if (!std::filesystem::exists(filePath))
    {
        std::cout << "File not found: " << filePath << ". Creating a new file." << std::endl;
        std::ofstream newFile(filePath);
        if (!newFile.is_open())
        {
            std::cerr << "Failed to create file: " << filePath << std::endl;
            return;
        }
        newFile.close();
    }

    nlohmann::json jsonLayers;

    for (const auto& layer : layers)
    {
        nlohmann::json jsonLayer;
        jsonLayer["name"] = layer.m_Name;

        for (const auto& row : layer.m_Tiles)
        {
            nlohmann::json jsonRow;
            for (const auto& tile : row)
            {
                nlohmann::json jsonTile;
                jsonTile["color"] = { tile.m_Color.r, tile.m_Color.g, tile.m_Color.b, tile.m_Color.a };
                jsonTile["use_texture"] = tile.m_UseTexture;
                jsonTile["opacity"] = tile.m_Opacity;
                jsonTile["texture_index"] = tile.m_TextureIndex;

                jsonRow.push_back(jsonTile);
            }
            jsonLayer["tiles"].push_back(jsonRow);
        }
        jsonLayers.push_back(jsonLayer);
    }

    std::ofstream file(filePath);
    if (file.is_open())
    {
        file << jsonLayers.dump(4); // Pretty print with 4 spaces
        file.close();
    }
    else
    {
        std::cerr << "Failed to open file for writing: " << filePath << std::endl;
    }
}

std::vector<TileLayer> TileSerializer::Deserialize(const std::string& filePath)
{
    std::vector<TileLayer> layers;
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file for reading: " << filePath << std::endl;
        return layers;
    }

    nlohmann::json jsonLayers;
    file >> jsonLayers;

    for (const auto& jsonLayer : jsonLayers)
    {
        TileLayer layer;
        layer.m_Name = jsonLayer.at("name").get<std::string>();

        for (const auto& jsonRow : jsonLayer.at("tiles"))
        {
            std::vector<Tile> row;
            for (const auto& jsonTile : jsonRow)
            {
                Tile tile;
                auto color = jsonTile.at("color").get<std::vector<float>>();
                tile.m_Color = glm::vec4(color[0], color[1], color[2], color[3]);
                tile.m_UseTexture = jsonTile.at("use_texture").get<bool>();
                tile.m_Opacity = jsonTile.at("opacity").get<float>();
                tile.m_TextureIndex = jsonTile.at("texture_index").get<int>();

                row.push_back(tile);
            }
            layer.m_Tiles.push_back(row);
        }
        layers.push_back(layer);
    }

    return layers;
}
