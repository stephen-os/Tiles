#include "Domain/ProjectSerializer.h"

#include <fstream>
#include <vector>
#include <cstring>
#include <string>

#include "json.hpp"
#include <miniz.h>

#include "Domain/Project.h"
#include "Domain/TextureAtlas.h"
#include "Domain/Constants.h"
#include "Core/Logger.h"

namespace Tiles
{
    namespace
    {
        constexpr const char* ManifestEntry = "manifest.json";

        // Reads an entire file into a byte buffer (binary, no newline translation).
        bool ReadFileBytes(const std::filesystem::path& path, std::vector<uint8_t>& out)
        {
            std::ifstream file(path, std::ios::binary | std::ios::ate);
            if (!file)
                return false;

            std::streamsize size = file.tellg();
            if (size < 0)
                return false;

            out.resize(static_cast<size_t>(size));
            file.seekg(0);
            if (size > 0)
                file.read(reinterpret_cast<char*>(out.data()), size);

            return !file.bad();
        }

        bool WriteFileBytes(const std::filesystem::path& path, const void* data, size_t size)
        {
            std::ofstream file(path, std::ios::binary | std::ios::trunc);
            if (!file)
                return false;

            file.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
            return file.good();
        }

        // Loads the legacy path-referencing JSON format so pre-container projects
        // still open. Save always writes the self-contained container.
        std::expected<std::shared_ptr<Project>, Error> LoadLegacyJSON(const std::vector<uint8_t>& bytes)
        {
            try
            {
                nlohmann::json jsonProject = nlohmann::json::parse(bytes);
                std::shared_ptr<Project> project = Project::FromJSON(jsonProject);
                if (!project)
                    return std::unexpected(Error{ ErrorCode::ReadFailure, "Invalid project file format." });
                return project;
            }
            catch (const std::exception& e)
            {
                return std::unexpected(Error{ ErrorCode::ReadFailure, std::string("Invalid project file format: ") + e.what() });
            }
        }
    }

    std::expected<void, Error> ProjectSerializer::Save(const Project& project, const std::filesystem::path& path)
    {
        auto directory = path.parent_path();
        if (!directory.empty() && !std::filesystem::exists(directory))
            std::filesystem::create_directories(directory);

        mz_zip_archive zip;
        std::memset(&zip, 0, sizeof(zip));
        if (!mz_zip_writer_init_heap(&zip, 0, 0))
            return std::unexpected(Error{ ErrorCode::WriteFailure, "Failed to initialize the project archive." });

        // Build the manifest as we embed atlas images. One entry per atlas keeps
        // atlas indices (referenced by tiles) stable; the image is embedded when
        // the atlas has a texture.
        nlohmann::json manifest;
        manifest[JSON::Project::Name] = project.GetProjectName();
        manifest[JSON::Project::LayerStack] = project.GetLayerStack().ToJSON();

        const ExportRegion& region = project.GetExportRegion();
        manifest[JSON::Region::Object] = {
            { JSON::Region::X, region.Min.x },
            { JSON::Region::Y, region.Min.y },
            { JSON::Region::Width, region.Size.x },
            { JSON::Region::Height, region.Size.y },
            { JSON::Region::Enabled, region.Enabled },
        };

        nlohmann::json atlasArray = nlohmann::json::array();
        const auto& atlases = project.GetTextureAtlases();
        for (size_t i = 0; i < atlases.size(); ++i)
        {
            const auto& atlas = atlases[i];
            if (!atlas)
                continue;

            nlohmann::json jsonAtlas;
            jsonAtlas[JSON::Atlas::Width] = atlas->GetWidth();
            jsonAtlas[JSON::Atlas::Height] = atlas->GetHeight();

            if (atlas->HasImage())
            {
                // Embed the atlas's source image bytes directly -- no GPU readback
                // or re-encoding -- so the saved file never depends on the original
                // image's path.
                const std::vector<uint8_t>& imageBytes = atlas->GetImageBytes();
                std::string imageName = "atlas" + std::to_string(i) + ".img";
                if (!mz_zip_writer_add_mem(&zip, imageName.c_str(), imageBytes.data(), imageBytes.size(), MZ_NO_COMPRESSION))
                {
                    mz_zip_writer_end(&zip);
                    return std::unexpected(Error{ ErrorCode::WriteFailure, "Failed to add an atlas image to the archive." });
                }

                jsonAtlas[JSON::Atlas::Image] = imageName;
            }

            atlasArray.push_back(std::move(jsonAtlas));
        }
        manifest[JSON::Atlas::Array] = atlasArray;

        std::string manifestStr = manifest.dump(2);
        if (!mz_zip_writer_add_mem(&zip, ManifestEntry, manifestStr.data(), manifestStr.size(), MZ_DEFAULT_COMPRESSION))
        {
            mz_zip_writer_end(&zip);
            return std::unexpected(Error{ ErrorCode::WriteFailure, "Failed to add the manifest to the archive." });
        }

        void* archiveBuffer = nullptr;
        size_t archiveSize = 0;
        if (!mz_zip_writer_finalize_heap_archive(&zip, &archiveBuffer, &archiveSize))
        {
            mz_zip_writer_end(&zip);
            return std::unexpected(Error{ ErrorCode::WriteFailure, "Failed to finalize the project archive." });
        }

        bool wrote = WriteFileBytes(path, archiveBuffer, archiveSize);
        mz_free(archiveBuffer);
        mz_zip_writer_end(&zip);

        if (!wrote)
            return std::unexpected(Error{ ErrorCode::WriteFailure, "Failed to write the project file." });

        return {};
    }

    std::expected<std::shared_ptr<Project>, Error> ProjectSerializer::Load(const std::filesystem::path& path)
    {
        std::vector<uint8_t> fileBytes;
        if (!ReadFileBytes(path, fileBytes))
            return std::unexpected(Error{ ErrorCode::ReadFailure, "Failed to open file for reading." });

        // A ZIP container starts with "PK"; anything else is treated as a legacy
        // JSON project.
        const bool isContainer = fileBytes.size() >= 2 && fileBytes[0] == 'P' && fileBytes[1] == 'K';
        if (!isContainer)
            return LoadLegacyJSON(fileBytes);

        mz_zip_archive zip;
        std::memset(&zip, 0, sizeof(zip));
        if (!mz_zip_reader_init_mem(&zip, fileBytes.data(), fileBytes.size(), 0))
            return std::unexpected(Error{ ErrorCode::ReadFailure, "Failed to open the project archive." });

        size_t manifestSize = 0;
        void* manifestData = mz_zip_reader_extract_file_to_heap(&zip, ManifestEntry, &manifestSize, 0);
        if (!manifestData)
        {
            mz_zip_reader_end(&zip);
            return std::unexpected(Error{ ErrorCode::ReadFailure, "Project archive is missing its manifest." });
        }

        std::shared_ptr<Project> project;
        try
        {
            nlohmann::json manifest = nlohmann::json::parse(
                std::string(reinterpret_cast<const char*>(manifestData), manifestSize));
            mz_free(manifestData);

            if (!manifest.contains(JSON::Project::LayerStack))
            {
                mz_zip_reader_end(&zip);
                return std::unexpected(Error{ ErrorCode::ReadFailure, "Invalid project file format." });
            }

            std::string name = manifest.value(JSON::Project::Name, "Untitled Project");
            project = std::make_shared<Project>(name);
            project->GetLayerStack() = LayerStack::FromJSON(manifest.at(JSON::Project::LayerStack));

            if (manifest.contains(JSON::Region::Object))
            {
                const auto& regionJson = manifest[JSON::Region::Object];
                ExportRegion& region = project->GetExportRegion();
                region.Min = { regionJson.value(JSON::Region::X, 0), regionJson.value(JSON::Region::Y, 0) };
                region.Size = { regionJson.value(JSON::Region::Width, 16), regionJson.value(JSON::Region::Height, 16) };
                region.Enabled = regionJson.value(JSON::Region::Enabled, false);
            }

            if (manifest.contains(JSON::Atlas::Array))
            {
                for (const auto& jsonAtlas : manifest[JSON::Atlas::Array])
                {
                    int gridWidth = jsonAtlas.value(JSON::Atlas::Width, 0);
                    int gridHeight = jsonAtlas.value(JSON::Atlas::Height, 0);
                    if (gridWidth <= 0 || gridHeight <= 0)
                        continue;

                    std::string imageName = jsonAtlas.value(JSON::Atlas::Image, "");
                    if (imageName.empty())
                    {
                        // Imageless atlas: preserve the slot so indices line up.
                        project->AddTextureAtlas(TextureAtlas::Create(gridWidth, gridHeight));
                        continue;
                    }

                    size_t imageSize = 0;
                    void* imageData = mz_zip_reader_extract_file_to_heap(&zip, imageName.c_str(), &imageSize, 0);
                    if (!imageData)
                    {
                        TILES_ENGINE_WARN("ProjectSerializer::Load: Missing embedded atlas image '{}'", imageName);
                        project->AddTextureAtlas(TextureAtlas::Create(gridWidth, gridHeight));
                        continue;
                    }

                    std::vector<uint8_t> imageBytes(
                        reinterpret_cast<uint8_t*>(imageData),
                        reinterpret_cast<uint8_t*>(imageData) + imageSize);
                    mz_free(imageData);

                    project->AddTextureAtlas(TextureAtlas::Create(std::move(imageBytes), gridWidth, gridHeight));
                }
            }
        }
        catch (const std::exception& e)
        {
            mz_zip_reader_end(&zip);
            return std::unexpected(Error{ ErrorCode::ReadFailure, std::string("Invalid project file format: ") + e.what() });
        }

        mz_zip_reader_end(&zip);
        return project;
    }
}
