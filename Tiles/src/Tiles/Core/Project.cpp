#include "Project.h"
#include <iostream>
#include <filesystem>

void Project::Save(const std::string& path, const Lumina::Ref<TileLayer>& tileLayer, const Lumina::Ref<Lumina::TextureAtlas>& atlas)
{
    if (!tileLayer || !atlas)
    {
        std::cerr << "Error: Invalid TileLayer or TextureAtlas reference." << std::endl;
        return;
    }

    nlohmann::json jsonProject;
    
    // Atlas
    jsonProject["atlas_path"] = atlas->GetPath();
    jsonProject["atlas_grid_width"] = atlas->GetGridWidth();
    jsonProject["atlas_grid_height"] = atlas->GetGridHeight();

    // TileLayer
    jsonProject["tilelayer_width"] = tileLayer->LayerWidth();
    jsonProject["tilelayer_height"] = tileLayer->LayerHeight();

    // Serialize TileLayer
    nlohmann::json jsonLayers = nlohmann::json::array();
    for (size_t layer = 0; layer < tileLayer->LayerSize(); layer++)
    {
        nlohmann::json jsonLayer;
        jsonLayer["name"] = tileLayer->GetLayerName(layer);

        nlohmann::json jsonTiles = nlohmann::json::array();
        for (size_t y = 0; y < tileLayer->LayerHeight(); y++)
        {
            nlohmann::json jsonRow = nlohmann::json::array();
            for (size_t x = 0; x < tileLayer->LayerWidth(); x++)
            {
                TileData tile = tileLayer->GetTile(layer, y, x);
                jsonRow.push_back({
                    {"texture_index", tile.TextureIndex}
                    });
            }
            jsonTiles.push_back(jsonRow);
        }
        jsonLayer["tiles"] = jsonTiles;
        jsonLayers.push_back(jsonLayer);
    }

    jsonProject["layers"] = jsonLayers;

    std::ofstream file(path);
    if (file.is_open())
    {
        file << jsonProject.dump(4);
        file.close();
    }
    else
    {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
    }
}

void Project::Load(const std::string& path, Lumina::Ref<TileLayer>& tileLayer, Lumina::Ref<Lumina::TextureAtlas>& atlas)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file for reading: " << path << std::endl;
        return;
    }

    nlohmann::json jsonProject;
    file >> jsonProject;
    file.close();

    // Load TextureAtlas
    std::string atlasPath = jsonProject.value("atlas_path", "");
    int atlasWidth = jsonProject.value("atlas_grid_width", 0);
    int atlasHeight = jsonProject.value("atlas_grid_height", 0);

    atlas->CreateAtlas(atlasPath, atlasWidth, atlasHeight);

    size_t layerWidth = jsonProject.value("tilelayer_width", 0);
	size_t layerHeight = jsonProject.value("tilelayer_height", 0);
	tileLayer->Create(layerWidth, layerHeight);

    // Load TileLayers
    for (const auto& jsonLayer : jsonProject["layers"])
    {
        std::string layerName = jsonLayer.at("name").get<std::string>();
        tileLayer->AddLayer(layerName);
        size_t layerIndex = tileLayer->LayerSize() - 1;

        for (size_t y = 0; y < layerWidth; y++)
        {
            for (size_t x = 0; x < layerWidth; x++)
            {
                TileData& tile = tileLayer->GetTile(layerIndex, y, x);
                tile.TextureIndex = jsonLayer["tiles"][y][x]["texture_index"].get<size_t>();
            }
        }
    }
}