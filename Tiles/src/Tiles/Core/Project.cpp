#include "Project.h"

#include "Layer.h"
#include "Tile.h"

#include <filesystem>
#include <fstream>

#include "spdlog/spdlog.h"

#include "json.hpp"

namespace Tiles
{

    void Project::Save(const std::string& path, const Shared<Layers>& layers, const Shared<Lumina::TextureAtlas>& atlas)
    {
        if (!layers || !atlas)
        {
            spdlog::error("[Project] -> Save: Invalid TileLayer or TextureAtlas reference.");
            return;
        }

        nlohmann::json jsonProject;

        // Atlas
        jsonProject["atlas_path"] = atlas->GetTexture()->GetPath();
        jsonProject["atlas_width"] = atlas->GetWidth();
        jsonProject["atlas_height"] = atlas->GetHeight();

        // TileLayer
        jsonProject["layers_width"] = layers->GetWidth();
        jsonProject["layers_height"] = layers->GetHeight();
        jsonProject["layers_name"] = layers->GetName();

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
                    jsonRow.push_back({{"texture_index", tile.GetTextureIndex()}});
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
            spdlog::error("[Project] -> Save: Failed to open file for writing: {}", path);
        }
    }

    void Project::Load(const std::string& path, Shared<Layers>& layers, Shared<Lumina::TextureAtlas>& atlas)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            spdlog::error("[Project] -> Load: Failed to open file for reading: {}", path);
            return;
        }

        nlohmann::json jsonProject;
        file >> jsonProject;
        file.close();

        // Load Texture Atlas
        std::string atlasPath = jsonProject.value("atlas_path", "");
        int atlasWidth = jsonProject.value("atlas_width", 0);
        int atlasHeight = jsonProject.value("atlas_height", 0);

        // atlas = MakeShared<Lumina::TextureAtlas>(atlasPath, atlasWidth, atlasHeight);

		atlas->Resize(atlasWidth, atlasHeight);
		atlas->SetTexture(atlasPath);

        // layers needs a create method that resets all attributes

        size_t layerWidth = jsonProject.value("layers_width", 0);
        size_t layerHeight = jsonProject.value("layers_height", 0);
        std::string layerName = jsonProject.value("layers_name", "");
        layers->ClearAllLayers();
        layers->Resize(layerWidth, layerHeight);
        layers->SetName(layerName);

        // Load TileLayers
        for (const auto& jsonLayer : jsonProject["layers"])
        {
            std::string layerName = jsonLayer.at("name").get<std::string>();
            Layer layer(layers->GetWidth(), layers->GetHeight(), layerName);

            for (size_t y = 0; y < layer.GetHeight(); y++)
            {
                for (size_t x = 0; x < layer.GetWidth(); x++)
                {
                    Tile& tile = layer.GetTile(y, x);
                    tile.SetTextureIndex(jsonLayer["tiles"][y][x]["texture_index"].get<int>());
                }
            }

            layers->InsertLayer(layers->GetSize(), layer);
        }
    }

}