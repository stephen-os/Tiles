#include "Domain/ProjectSerializer.h"

#include <cmath>
#include <cstring>
#include <fstream>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

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

		// Defined below; brings a pre-AtlasId manifest up to the current schema in place.
		void MigrateLegacyTilesToV2(nlohmann::json& manifest);

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

		// Writes a byte buffer to a file (binary, truncating any existing content).
		bool WriteFileBytes(const std::filesystem::path& path, const void* data, size_t size)
		{
			std::ofstream file(path, std::ios::binary | std::ios::trunc);
			if (!file)
				return false;

			file.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
			return file.good();
		}

		// Ends a miniz heap writer on every exit path (early return or a thrown
		// exception), so its internal allocations can't leak. Construct one only
		// after mz_zip_writer_init_heap has succeeded.
		struct ZipWriterGuard
		{
			mz_zip_archive* Zip;
			~ZipWriterGuard() { mz_zip_writer_end(Zip); }
		};

		// Loads the legacy path-referencing JSON format so pre-container projects
		// still open. Save always writes the self-contained container.
		std::expected<std::shared_ptr<Project>, Error> LoadLegacyJSON(const std::vector<uint8_t>& bytes)
		{
			try
			{
				nlohmann::json jsonProject = nlohmann::json::parse(bytes);

				// Pre-container files predate AtlasId; migrate their tiles before
				// FromJSON reads them so textures survive the load.
				if (jsonProject.value(JSON::Project::Version, 1) < 2)
					MigrateLegacyTilesToV2(jsonProject);

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

		// Rewrites a pre-AtlasId manifest in place so the loader can read it as the
		// current schema. Each tile's positional atlas index + cached UV rectangle
		// becomes a stable atlas id (index + 1, matching the order the loader re-mints
		// ids) plus a cell index (recovered from the UV via the atlas grid).
		// Untextured or dangling references collapse to the invalid id.
		void MigrateLegacyTilesToV2(nlohmann::json& manifest)
		{
			// Grid dimensions per atlas, positionally -- the UV->cell inverse needs them.
			std::vector<std::pair<int, int>> atlasGrids;
			if (manifest.contains(JSON::Atlas::Array))
				for (const auto& jsonAtlas : manifest[JSON::Atlas::Array])
					atlasGrids.emplace_back(
						jsonAtlas.value(JSON::Atlas::Width, 1),
						jsonAtlas.value(JSON::Atlas::Height, 1));

			// Converts one tile object's legacy atlas fields to the new id + cell pair.
			auto migrateTile = [&atlasGrids](nlohmann::json& tile)
			{
				if (!tile.contains(JSON::Tile::AtlasIndex))
					return;

				size_t oldIndex = tile.value(JSON::Tile::AtlasIndex, SIZE_MAX);
				if (oldIndex >= atlasGrids.size())
				{
					tile[JSON::Tile::AtlasId] = 0;   // AtlasId::Invalid
					tile[JSON::Tile::CellIndex] = 0;
					return;
				}

				const auto [gridWidth, gridHeight] = atlasGrids[oldIndex];
				int cell = 0;
				if (tile.contains(JSON::Tile::TextureCoords))
				{
					const auto& uv = tile[JSON::Tile::TextureCoords];
					if (uv.size() >= 2)
					{
						int col = static_cast<int>(std::lround(uv[0].get<float>() * gridWidth));
						int row = static_cast<int>(std::lround(uv[1].get<float>() * gridHeight));
						cell = row * gridWidth + col;
					}
				}

				tile[JSON::Tile::AtlasId] = oldIndex + 1;   // positional index -> stable id
				tile[JSON::Tile::CellIndex] = cell;
			};

			if (!manifest.contains(JSON::Project::LayerStack))
				return;

			nlohmann::json& layerStack = manifest[JSON::Project::LayerStack];
			if (!layerStack.contains(JSON::LayerStack::TileLayers))
				return;

			for (auto& layer : layerStack[JSON::LayerStack::TileLayers])
			{
				if (!layer.contains(JSON::TileLayer::Tiles))
					continue;

				// The tile list is either sparse entries or legacy dense rows; both
				// bottom out at the tile objects we migrate.
				for (auto& element : layer[JSON::TileLayer::Tiles])
				{
					if (element.is_array())
						for (auto& tile : element)
							migrateTile(tile);
					else
						migrateTile(element);
				}
			}
		}
	}

	std::expected<void, Error> ProjectSerializer::Save(const Project& project, const std::filesystem::path& path)
	{
		auto directory = path.parent_path();
		if (!directory.empty())
		{
			std::error_code dirEc;
			std::filesystem::create_directories(directory, dirEc);
			if (dirEc)
				return std::unexpected(Error{ ErrorCode::WriteFailure, "Failed to create the project directory: " + dirEc.message() });
		}

		mz_zip_archive zip;
		std::memset(&zip, 0, sizeof(zip));
		if (!mz_zip_writer_init_heap(&zip, 0, 0))
			return std::unexpected(Error{ ErrorCode::WriteFailure, "Failed to initialize the project archive." });

		// Ends the writer on every path below -- including a json exception thrown
		// while building or dumping the manifest -- so it can never leak.
		ZipWriterGuard zipGuard{ &zip };

		try
		{
			// Build the manifest as we embed atlas images. One entry per atlas keeps
			// atlas indices (referenced by tiles) stable; the image is embedded when
			// the atlas has one.
			nlohmann::json manifest;
			manifest[JSON::Project::Name] = project.GetProjectName();
			manifest[JSON::Project::Version] = 2;
			manifest[JSON::Project::NextAtlasId] = project.GetNextAtlasId();
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
				jsonAtlas[JSON::Atlas::Id] = static_cast<uint32_t>(atlas->GetId());
				jsonAtlas[JSON::Atlas::Name] = atlas->GetName();
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
						return std::unexpected(Error{ ErrorCode::WriteFailure, "Failed to add an atlas image to the archive." });

					jsonAtlas[JSON::Atlas::Image] = imageName;
				}

				atlasArray.push_back(std::move(jsonAtlas));
			}
			manifest[JSON::Atlas::Array] = atlasArray;

			std::string manifestStr = manifest.dump(2);
			if (!mz_zip_writer_add_mem(&zip, ManifestEntry, manifestStr.data(), manifestStr.size(), MZ_DEFAULT_COMPRESSION))
				return std::unexpected(Error{ ErrorCode::WriteFailure, "Failed to add the manifest to the archive." });

			// Finalize into a heap buffer, then write it to a sibling temp file and
			// rename over the target. rename is atomic on one volume, so a failed or
			// partial write can never truncate the previous good save.
			std::filesystem::path tempPath = path;
			tempPath += ".tmp";

			void* archiveBuffer = nullptr;
			size_t archiveSize = 0;
			if (!mz_zip_writer_finalize_heap_archive(&zip, &archiveBuffer, &archiveSize))
				return std::unexpected(Error{ ErrorCode::WriteFailure, "Failed to finalize the project archive." });

			bool wrote = WriteFileBytes(tempPath, archiveBuffer, archiveSize);
			mz_free(archiveBuffer);

			if (!wrote)
			{
				std::error_code removeEc;
				std::filesystem::remove(tempPath, removeEc);
				return std::unexpected(Error{ ErrorCode::WriteFailure, "Failed to write the project file." });
			}

			std::error_code renameEc;
			std::filesystem::rename(tempPath, path, renameEc);
			if (renameEc)
			{
				std::error_code removeEc;
				std::filesystem::remove(tempPath, removeEc);
				return std::unexpected(Error{ ErrorCode::WriteFailure, "Failed to replace the project file: " + renameEc.message() });
			}

			return {};
		}
		catch (const std::exception& e)
		{
			return std::unexpected(Error{ ErrorCode::WriteFailure, std::string("Failed to encode the project file: ") + e.what() });
		}
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

		// Copy the manifest bytes out and free the miniz buffer immediately, so a
		// throw from the parse below can't leak it.
		std::string manifestStr(reinterpret_cast<const char*>(manifestData), manifestSize);
		mz_free(manifestData);

		std::shared_ptr<Project> project;
		try
		{
			nlohmann::json manifest = nlohmann::json::parse(manifestStr);

			if (!manifest.contains(JSON::Project::LayerStack))
			{
				mz_zip_reader_end(&zip);
				return std::unexpected(Error{ ErrorCode::ReadFailure, "Invalid project file format." });
			}

			// Bring a pre-AtlasId manifest up to the current schema before any of
			// it is read into objects, so the load path below is version-agnostic.
			if (manifest.value(JSON::Project::Version, 1) < 2)
				MigrateLegacyTilesToV2(manifest);

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

					// Rebuild the atlas from its embedded image, falling back to an
					// imageless slot when the image is absent or fails to extract.
					std::shared_ptr<TextureAtlas> atlas;
					std::string imageName = jsonAtlas.value(JSON::Atlas::Image, "");
					if (imageName.empty())
					{
						atlas = TextureAtlas::Create(gridWidth, gridHeight);
					}
					else
					{
						size_t imageSize = 0;
						void* imageData = mz_zip_reader_extract_file_to_heap(&zip, imageName.c_str(), &imageSize, 0);
						if (imageData)
						{
							std::vector<uint8_t> imageBytes(
								reinterpret_cast<uint8_t*>(imageData),
								reinterpret_cast<uint8_t*>(imageData) + imageSize);
							mz_free(imageData);
							atlas = TextureAtlas::Create(std::move(imageBytes), gridWidth, gridHeight);
						}
						else
						{
							TILES_ENGINE_WARN("ProjectSerializer::Load: Missing embedded atlas image '{}'", imageName);
							atlas = TextureAtlas::Create(gridWidth, gridHeight);
						}
					}

					// AddTextureAtlas mints an id; override it with the saved one so
					// tile references survive a save/load round-trip. Migrated files
					// have no saved id and keep the minted 1..N sequence.
					project->AddTextureAtlas(atlas);
					if (jsonAtlas.contains(JSON::Atlas::Id))
						atlas->SetId(static_cast<AtlasId>(jsonAtlas[JSON::Atlas::Id].get<uint32_t>()));
					atlas->SetName(jsonAtlas.value(JSON::Atlas::Name, ""));
				}

				// Restore the id allocator past every id just loaded (migrated files
				// leave it at the minted high-water mark via the default).
				project->SetNextAtlasId(manifest.value(JSON::Project::NextAtlasId, project->GetNextAtlasId()));
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
