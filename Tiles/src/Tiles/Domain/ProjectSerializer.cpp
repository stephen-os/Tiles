#include "Domain/ProjectSerializer.h"

#include <fstream>
#include <vector>
#include <cstring>
#include <string>

#include "json.hpp"
#include <miniz.h>
#include <stb_image.h>

#include "Domain/Project.h"
#include "Core/Constants.h"
#include "Core/Log.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureAtlas.h"

namespace Tiles
{
    namespace
    {
        constexpr const char* ManifestEntry = "manifest.json";

        /// Reads an entire file into a byte buffer (binary, no newline translation).
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

        /// Loads the legacy path-referencing JSON format so pre-container projects
        /// still open. Save always writes the self-contained container.
        ProjectResult LoadLegacyJSON(const std::vector<uint8_t>& bytes, std::shared_ptr<Project>& outProject)
        {
            try
            {
                nlohmann::json jsonProject = nlohmann::json::parse(bytes);
                outProject = Project::FromJSON(jsonProject);
                if (!outProject)
                    return { false, "Invalid project file format." };
                return { true, "Project loaded successfully." };
            }
            catch (const std::exception& e)
            {
                return { false, std::string("Invalid project file format: ") + e.what() };
            }
        }
    }

    ProjectResult ProjectSerializer::Save(const Project& project, const std::filesystem::path& path)
    {
        auto directory = path.parent_path();
        if (!directory.empty() && !std::filesystem::exists(directory))
            std::filesystem::create_directories(directory);

        mz_zip_archive zip;
        std::memset(&zip, 0, sizeof(zip));
        if (!mz_zip_writer_init_heap(&zip, 0, 0))
            return { false, "Failed to initialize the project archive." };

        // Build the manifest as we embed atlas images. One entry per atlas keeps
        // atlas indices (referenced by tiles) stable; the image is embedded when
        // the atlas has a texture.
        nlohmann::json manifest;
        manifest[JSON::Project::Name] = project.GetProjectName();
        manifest[JSON::Project::LayerStack] = project.GetLayerStack().ToJSON();

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

            if (auto texture = atlas->GetTexture())
            {
                // Read the atlas back from the GPU and encode it to a PNG embedded
                // in the archive, so the saved file never depends on the original
                // image's path.
                std::vector<uint8_t> pixels = texture->ReadPixels();
                size_t pngSize = 0;
                void* png = tdefl_write_image_to_png_file_in_memory(
                    pixels.data(),
                    static_cast<int>(texture->GetWidth()),
                    static_cast<int>(texture->GetHeight()),
                    4, &pngSize);
                if (!png)
                {
                    mz_zip_writer_end(&zip);
                    return { false, "Failed to encode an atlas image." };
                }

                std::string imageName = "atlas" + std::to_string(i) + ".png";
                mz_bool added = mz_zip_writer_add_mem(&zip, imageName.c_str(), png, pngSize, MZ_NO_COMPRESSION);
                mz_free(png);
                if (!added)
                {
                    mz_zip_writer_end(&zip);
                    return { false, "Failed to add an atlas image to the archive." };
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
            return { false, "Failed to add the manifest to the archive." };
        }

        void* archiveBuffer = nullptr;
        size_t archiveSize = 0;
        if (!mz_zip_writer_finalize_heap_archive(&zip, &archiveBuffer, &archiveSize))
        {
            mz_zip_writer_end(&zip);
            return { false, "Failed to finalize the project archive." };
        }

        bool wrote = WriteFileBytes(path, archiveBuffer, archiveSize);
        mz_free(archiveBuffer);
        mz_zip_writer_end(&zip);

        if (!wrote)
            return { false, "Failed to write the project file." };

        return { true, "Project saved successfully." };
    }

    ProjectResult ProjectSerializer::Load(const std::filesystem::path& path, std::shared_ptr<Project>& outProject)
    {
        std::vector<uint8_t> fileBytes;
        if (!ReadFileBytes(path, fileBytes))
            return { false, "Failed to open file for reading." };

        // A ZIP container starts with "PK"; anything else is treated as a legacy
        // JSON project.
        const bool isContainer = fileBytes.size() >= 2 && fileBytes[0] == 'P' && fileBytes[1] == 'K';
        if (!isContainer)
            return LoadLegacyJSON(fileBytes, outProject);

        mz_zip_archive zip;
        std::memset(&zip, 0, sizeof(zip));
        if (!mz_zip_reader_init_mem(&zip, fileBytes.data(), fileBytes.size(), 0))
            return { false, "Failed to open the project archive." };

        size_t manifestSize = 0;
        void* manifestData = mz_zip_reader_extract_file_to_heap(&zip, ManifestEntry, &manifestSize, 0);
        if (!manifestData)
        {
            mz_zip_reader_end(&zip);
            return { false, "Project archive is missing its manifest." };
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
                return { false, "Invalid project file format." };
            }

            std::string name = manifest.value(JSON::Project::Name, "Untitled Project");
            project = std::make_shared<Project>(name);
            project->GetLayerStack() = LayerStack::FromJSON(manifest.at(JSON::Project::LayerStack));

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
                        // Textureless atlas: preserve the slot so indices line up.
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

                    int width = 0, height = 0, channels = 0;
                    stbi_uc* pixels = stbi_load_from_memory(
                        reinterpret_cast<stbi_uc*>(imageData), static_cast<int>(imageSize),
                        &width, &height, &channels, 4);
                    mz_free(imageData);

                    if (!pixels)
                    {
                        TILES_ENGINE_WARN("ProjectSerializer::Load: Failed to decode atlas image '{}'", imageName);
                        project->AddTextureAtlas(TextureAtlas::Create(gridWidth, gridHeight));
                        continue;
                    }

                    auto texture = Texture::CreateFromData(pixels, static_cast<uint32_t>(width), static_cast<uint32_t>(height), 4);
                    stbi_image_free(pixels);

                    if (texture)
                        project->AddTextureAtlas(TextureAtlas::Create(texture, gridWidth, gridHeight));
                    else
                        project->AddTextureAtlas(TextureAtlas::Create(gridWidth, gridHeight));
                }
            }
        }
        catch (const std::exception& e)
        {
            mz_zip_reader_end(&zip);
            return { false, std::string("Invalid project file format: ") + e.what() };
        }

        mz_zip_reader_end(&zip);
        outProject = project;
        return { true, "Project loaded successfully." };
    }
}
