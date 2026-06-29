#include "JsonProjectRepository.h"

#include <fstream>
#include <json.hpp>
#include <cmath>
#include <algorithm>

namespace Tiles::Infrastructure
{
    using json = nlohmann::json;

    // Security limits for untrusted input validation
    namespace Limits
    {
        constexpr uint32_t MaxAtlasIndex = 1000;
        constexpr uint32_t MaxTileIndex = 100000;
        constexpr uint32_t MaxGridDimension = 10000;
        constexpr size_t MaxLayers = 1000;
        constexpr size_t MaxAtlasReferences = 1000;
        constexpr size_t MaxTilesPerLayer = 10'000'000;
        constexpr float MaxRotation = 360000.0f;
    }

    // Serialization helpers
    namespace
    {
        json TileDataToJson(const Domain::TileData& tile)
        {
            return json{
                {"atlasIndex", tile.AtlasIndex},
                {"tileIndex", tile.TileIndex},
                {"rotation", tile.Rotation},
                {"flipX", tile.FlipX},
                {"flipY", tile.FlipY}
            };
        }

        Domain::TileData TileDataFromJson(const json& j)
        {
            // Validate and clamp all values from untrusted input
            uint32_t atlasIndex = j.value("atlasIndex", 0u);
            uint32_t tileIndex = j.value("tileIndex", 0u);
            float rotation = j.value("rotation", 0.0f);

            // Clamp to safe limits
            atlasIndex = std::min(atlasIndex, Limits::MaxAtlasIndex);
            tileIndex = std::min(tileIndex, Limits::MaxTileIndex);

            // Handle NaN/Inf and clamp rotation
            if (!std::isfinite(rotation)) {
                rotation = 0.0f;
            }
            rotation = std::clamp(rotation, -Limits::MaxRotation, Limits::MaxRotation);

            return Domain::TileData{
                atlasIndex,
                tileIndex,
                rotation,
                j.value("flipX", false),
                j.value("flipY", false)
            };
        }

        json TileGridToJson(const Domain::TileGrid& grid)
        {
            json tilesArray = json::array();
            grid.ForEachNonEmptyTile([&](uint32_t x, uint32_t y, const Domain::TileData& tile) {
                tilesArray.push_back(json{
                    {"x", x},
                    {"y", y},
                    {"tile", TileDataToJson(tile)}
                });
            });

            return json{
                {"name", grid.GetName()},
                {"width", grid.GetWidth()},
                {"height", grid.GetHeight()},
                {"visible", grid.IsVisible()},
                {"tiles", tilesArray}
            };
        }

        std::unique_ptr<Domain::TileGrid> TileGridFromJson(const json& j)
        {
            // Validate and clamp dimensions
            uint32_t width = j.value("width", 16u);
            uint32_t height = j.value("height", 16u);

            width = std::clamp(width, 1u, Limits::MaxGridDimension);
            height = std::clamp(height, 1u, Limits::MaxGridDimension);

            auto grid = std::make_unique<Domain::TileGrid>(
                width,
                height,
                j.value("name", "Layer")
            );
            grid->SetVisible(j.value("visible", true));

            if (j.contains("tiles") && j["tiles"].is_array())
            {
                const auto& tiles = j["tiles"];
                // Limit tile count to prevent OOM
                size_t tileCount = std::min(tiles.size(), Limits::MaxTilesPerLayer);

                for (size_t i = 0; i < tileCount; ++i)
                {
                    const auto& tileEntry = tiles[i];
                    uint32_t x = tileEntry.value("x", 0u);
                    uint32_t y = tileEntry.value("y", 0u);

                    // Only set tile if within grid bounds
                    if (x < width && y < height && tileEntry.contains("tile"))
                    {
                        auto tile = TileDataFromJson(tileEntry["tile"]);
                        grid->SetTile(x, y, tile);
                    }
                }
            }

            return grid;
        }

        json ProjectToJson(const Domain::TileProject& project)
        {
            json layersArray = json::array();
            for (size_t i = 0; i < project.GetLayerCount(); ++i)
            {
                layersArray.push_back(TileGridToJson(project.GetLayer(i)));
            }

            return json{
                {"version", 1},
                {"name", project.GetName()},
                {"width", project.GetWidth()},
                {"height", project.GetHeight()},
                {"atlasReferences", project.GetAtlasReferences()},
                {"layers", layersArray}
            };
        }

        // Helper to load tiles into a layer with bounds checking
        void LoadTilesIntoLayer(Domain::TileGrid& layer, const json& layerJson)
        {
            if (!layerJson.contains("tiles") || !layerJson["tiles"].is_array())
                return;

            const auto& tiles = layerJson["tiles"];
            size_t tileCount = std::min(tiles.size(), Limits::MaxTilesPerLayer);
            uint32_t width = layer.GetWidth();
            uint32_t height = layer.GetHeight();

            for (size_t i = 0; i < tileCount; ++i)
            {
                const auto& tileEntry = tiles[i];
                uint32_t x = tileEntry.value("x", 0u);
                uint32_t y = tileEntry.value("y", 0u);

                // Only set tile if within grid bounds
                if (x < width && y < height && tileEntry.contains("tile"))
                {
                    auto tile = TileDataFromJson(tileEntry["tile"]);
                    layer.SetTile(x, y, tile);
                }
            }
        }

        std::unique_ptr<Domain::TileProject> ProjectFromJson(const json& j)
        {
            // Validate and clamp dimensions
            uint32_t width = j.value("width", 16u);
            uint32_t height = j.value("height", 16u);

            width = std::clamp(width, 1u, Limits::MaxGridDimension);
            height = std::clamp(height, 1u, Limits::MaxGridDimension);

            auto project = std::make_unique<Domain::TileProject>(
                width,
                height,
                j.value("name", "Untitled")
            );

            // Load atlas references with limit
            if (j.contains("atlasReferences") && j["atlasReferences"].is_array())
            {
                const auto& refs = j["atlasReferences"];
                size_t refCount = std::min(refs.size(), Limits::MaxAtlasReferences);

                for (size_t i = 0; i < refCount; ++i)
                {
                    if (refs[i].is_string())
                    {
                        project->AddAtlasReference(refs[i].get<std::string>());
                    }
                }
            }

            // Load layers with limit
            if (j.contains("layers") && j["layers"].is_array() && !j["layers"].empty())
            {
                const auto& layers = j["layers"];
                size_t layerCount = std::min(layers.size(), Limits::MaxLayers);

                bool firstLayer = true;
                for (size_t i = 0; i < layerCount; ++i)
                {
                    const auto& layerJson = layers[i];

                    if (firstLayer)
                    {
                        // Modify the existing first layer
                        auto& layer = project->GetLayer(0);
                        layer.SetName(layerJson.value("name", "Layer"));
                        layer.SetVisible(layerJson.value("visible", true));
                        LoadTilesIntoLayer(layer, layerJson);
                        firstLayer = false;
                    }
                    else
                    {
                        size_t idx = project->AddLayer(layerJson.value("name", "Layer"));
                        auto& layer = project->GetLayer(idx);
                        layer.SetVisible(layerJson.value("visible", true));
                        LoadTilesIntoLayer(layer, layerJson);
                    }
                }
            }

            return project;
        }
    }

    Domain::RepositoryResult<void> JsonProjectRepository::Save(
        const Domain::TileProject& project,
        const std::filesystem::path& path)
    {
        try
        {
            json j = ProjectToJson(project);

            std::ofstream file(path);
            if (!file.is_open())
            {
                return Domain::RepositoryError{
                    Domain::RepositoryError::Code::AccessDenied,
                    "Could not open file for writing: " + path.string()
                };
            }

            file << j.dump(2);
            file.close();

            return {};
        }
        catch (const std::exception& e)
        {
            return Domain::RepositoryError{
                Domain::RepositoryError::Code::IOError,
                std::string("Failed to save project: ") + e.what()
            };
        }
    }

    Domain::RepositoryResult<std::unique_ptr<Domain::TileProject>> JsonProjectRepository::Load(
        const std::filesystem::path& path)
    {
        try
        {
            if (!Exists(path))
            {
                return Domain::RepositoryError{
                    Domain::RepositoryError::Code::FileNotFound,
                    "File not found: " + path.string()
                };
            }

            std::ifstream file(path);
            if (!file.is_open())
            {
                return Domain::RepositoryError{
                    Domain::RepositoryError::Code::AccessDenied,
                    "Could not open file for reading: " + path.string()
                };
            }

            json j = json::parse(file);
            file.close();

            auto project = ProjectFromJson(j);
            project->SetFilePath(path);
            project->MarkAsSaved();

            return project;
        }
        catch (const json::parse_error& e)
        {
            return Domain::RepositoryError{
                Domain::RepositoryError::Code::InvalidFormat,
                std::string("Invalid JSON format: ") + e.what()
            };
        }
        catch (const std::exception& e)
        {
            return Domain::RepositoryError{
                Domain::RepositoryError::Code::IOError,
                std::string("Failed to load project: ") + e.what()
            };
        }
    }

    bool JsonProjectRepository::Exists(const std::filesystem::path& path) const
    {
        return std::filesystem::exists(path);
    }

    std::optional<std::filesystem::file_time_type> JsonProjectRepository::GetLastModified(
        const std::filesystem::path& path) const
    {
        if (!Exists(path))
            return std::nullopt;

        try
        {
            return std::filesystem::last_write_time(path);
        }
        catch (...)
        {
            return std::nullopt;
        }
    }
}
