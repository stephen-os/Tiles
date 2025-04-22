#include "Project.h"

#include "Layer.h"
#include "Tile.h"

#include <filesystem>
#include <fstream>

#include "Lumina/Core/Log.h"
#include "Lumina/Core/Assert.h"

#include "json.hpp"

namespace Tiles
{
    void Project::Save(const std::string& path, const Layers& layers, const Lumina::TextureAtlas& atlas)
    {
        nlohmann::json jsonProject;

        // Atlas
        jsonProject["atlas_path"] = atlas.GetTexture()->GetPath();
        jsonProject["atlas_width"] = atlas.GetWidth();
        jsonProject["atlas_height"] = atlas.GetHeight();

        // TileLayer meta
        jsonProject["layers_width"] = layers.GetWidth();
        jsonProject["layers_height"] = layers.GetHeight();
        jsonProject["layers_name"] = layers.GetName();

        nlohmann::json jsonLayers = nlohmann::json::array();
        for (size_t l = 0; l < layers.GetSize(); l++)
        {
            nlohmann::json jsonLayer;
            const Layer& layer = layers.GetLayer(l);
            jsonLayer["name"] = layer.GetName();

            nlohmann::json jsonTiles = nlohmann::json::array();
            for (size_t row = 0; row < layer.GetHeight(); row++)
            {
                nlohmann::json jsonRow = nlohmann::json::array();
                for (size_t col = 0; col < layer.GetWidth(); col++)
                {
                    const Tile tile = layer.GetTile(row, col);
                    glm::vec4 tint = tile.GetTintColor();

                    jsonRow.push_back({
                        { "texture_index", tile.GetTextureIndex() },
                        { "tint_color", { tint.r, tint.g, tint.b, tint.a } },
                        { "rotation", tile.GetRotation() }
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
			LUMINA_LOG_ERROR("Failed to open file for writing: {}", path);
        }

        size_t totalTiles = layers.GetSize() * layers.GetWidth() * layers.GetHeight();

        LUMINA_LOG_INFO("[Project::Save] Saved project to '{}'", path);
        LUMINA_LOG_INFO("Atlas Path: '{}', Size: {}x{}", atlas.GetTexture()->GetPath(), atlas.GetWidth(), atlas.GetHeight());
        LUMINA_LOG_INFO("Layers Name: '{}', Size: {}x{}, Layer Count: {}, Total Tiles: {}",
            layers.GetName(), layers.GetWidth(), layers.GetHeight(), layers.GetSize(), totalTiles);
    }

    void Project::Load(const std::string& path, Layers& layers, Lumina::TextureAtlas& atlas)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
			LUMINA_LOG_ERROR("Failed to open file for reading: {}", path);
            return;
        }

        nlohmann::json jsonProject;
        file >> jsonProject;
        file.close();

        // Texture Atlas
        std::string atlasPath = jsonProject.value("atlas_path", "");
        int atlasWidth = jsonProject.value("atlas_width", 0);
        int atlasHeight = jsonProject.value("atlas_height", 0);
        atlas.Resize(atlasWidth, atlasHeight);
        atlas.SetTexture(atlasPath);

        // TileLayer metadata
        size_t layerWidth = jsonProject.value("layers_width", 0);
        size_t layerHeight = jsonProject.value("layers_height", 0);
        std::string layerName = jsonProject.value("layers_name", "");
        layers.ClearAllLayers();
        layers.Resize(layerWidth, layerHeight);
        layers.SetName(layerName);

        // Layers
        for (const auto& jsonLayer : jsonProject["layers"])
        {
            std::string layerName = jsonLayer.at("name").get<std::string>();
            Layer layer(layers.GetWidth(), layers.GetHeight(), layerName);

            for (size_t row = 0; row < layer.GetHeight(); row++)
            {
                for (size_t col = 0; col < layer.GetWidth(); col++)
                {
                    Tile& tile = layer.GetTile(row, col);
                    const auto& jsonTile = jsonLayer["tiles"][row][col];

                    tile.SetTextureIndex(jsonTile.value("texture_index", 0));

                    auto tint = jsonTile.value("tint_color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f});
                    if (tint.size() == 4)
                        tile.SetTintColor(glm::vec4(tint[0], tint[1], tint[2], tint[3]));

                    tile.SetRotation(jsonTile.value("rotation", 0.0f));
                }
            }

            layers.InsertLayer(layers.GetSize(), layer);
        }

        size_t totalTiles = layers.GetSize() * layers.GetWidth() * layers.GetHeight();

        LUMINA_LOG_INFO("[Project::Load] Loaded project from '{}'", path);
        LUMINA_LOG_INFO("Atlas Path: '{}', Size: {}x{}", atlas.GetTexture()->GetPath(), atlas.GetWidth(), atlas.GetHeight());
        LUMINA_LOG_INFO("Layers Name: '{}', Size: {}x{}, Layer Count: {}, Total Tiles: {}",
            layers.GetName(), layers.GetWidth(), layers.GetHeight(), layers.GetSize(), totalTiles);
    }
}