#include "Project.h"

#include "Layer.h"
#include "Tile.h"

#include <iostream>
#include <filesystem>

void Project::Save(const std::string& path, const Lumina::Ref<Layers>& layers, const Lumina::Ref<Atlas>& atlas)
{
    if (!layers || !atlas)
    {
        std::cerr << "Error: Invalid TileLayer or TextureAtlas reference." << std::endl;
        return;
    }

    nlohmann::json jsonProject;
    
    // Atlas
    jsonProject["atlas_path"] = atlas->GetPath();
    jsonProject["atlas_width"] = atlas->GetWidth();
    jsonProject["atlas_height"] = atlas->GetHeight();

    // TileLayer
    jsonProject["layers_width"] = layers->GetWidth();
    jsonProject["layers_height"] = layers->GetHeight();

    // Serialize TileLayer
    nlohmann::json jsonLayers = nlohmann::json::array();
    for (size_t l = 0; l < layers->GetSize(); l++)
    {
        nlohmann::json jsonLayer;

        Layer& layer = layers->GetLayer(l);

        jsonLayer["name"] = layer.GetName();

        nlohmann::json jsonTiles = nlohmann::json::array();
        for (size_t y = 0; y < layer.GetHeight(); y++)
        {
            nlohmann::json jsonRow = nlohmann::json::array();
            for (size_t x = 0; x < layer.GetWidth(); x++)
            {
                Tile tile = layer.GetTile(y, x);
                jsonRow.push_back({
                    {"texture_used", tile.UseTexture()},
                    {"texture_index", tile.GetTextureIndex()}
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

void Project::Load(const std::string& path, Lumina::Ref<Layers>& layers, Lumina::Ref<Atlas>& atlas)
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

    atlas->Create(atlasPath, atlasWidth, atlasHeight);

    // layers needs a create method that resets all attributes
#if 0
    size_t layerWidth = jsonProject.value("tilelayer_width", 0);
	size_t layerHeight = jsonProject.value("tilelayer_height", 0);
	layers->Create(layerWidth, layerHeight);

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
#endif
}