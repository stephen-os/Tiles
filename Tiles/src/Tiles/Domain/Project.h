#pragma once

#include "Domain/Tile.h"
#include "Domain/TileLayer.h"
#include "Domain/LayerStack.h"
#include "Domain/TextureAtlas.h"

#include "json.hpp"

#include <glm/glm.hpp>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace Tiles
{
	// A user-defined rectangular export region in tile coordinates. When enabled
	// it defines what the exporter renders, and is drawn as a guide in the
	// viewport; otherwise export falls back to the painted content's bounds.
	struct ExportRegion
	{
		glm::ivec2 Min{ 0, 0 };       // bottom-left tile (inclusive)
		glm::ivec2 Size{ 16, 16 };    // size in tiles
		bool Enabled = false;
	};

	// A document: the layer stack, its texture atlases, and file/dirty
	// bookkeeping. A project with no file path is considered "new" (unsaved).
	class Project
	{
	public:
		// Reconstructs a project from the legacy path-referencing JSON format.
		// @return Null if the required layer-stack field is missing.
		[[nodiscard]] static std::shared_ptr<Project> FromJSON(const nlohmann::json& json);

		explicit Project(const std::string& name = "Untitled Project");
		~Project() = default;

		[[nodiscard]] const std::string& GetProjectName() const { return m_ProjectName; }
		[[nodiscard]] const std::filesystem::path& GetFilePath() const { return m_FilePath; }
		[[nodiscard]] auto GetLastAccessed() const { return m_LastAccessed; }
		[[nodiscard]] auto GetLastSaved() const { return m_LastSaved; }

		void SetProjectName(const std::string& name);
		void SetFilePath(const std::filesystem::path& path) { m_FilePath = path; }
		void UpdateLastAccessed() { m_LastAccessed = std::chrono::steady_clock::now(); }
		void UpdateLastSaved() { m_LastSaved = std::chrono::steady_clock::now(); }

		void MarkAsModified() { m_HasUnsavedChanges = true; }
		void MarkAsSaved() { m_HasUnsavedChanges = false; UpdateLastSaved(); }
		[[nodiscard]] bool IsNew() const { return m_FilePath.empty(); }
		[[nodiscard]] bool HasUnsavedChanges() const { return m_HasUnsavedChanges; }

		[[nodiscard]] std::vector<std::shared_ptr<TextureAtlas>>& GetTextureAtlases() { return m_TextureAtlases; }
		[[nodiscard]] const std::vector<std::shared_ptr<TextureAtlas>>& GetTextureAtlases() const { return m_TextureAtlases; }

		[[nodiscard]] LayerStack& GetLayerStack() { return m_LayerStack; }
		[[nodiscard]] const LayerStack& GetLayerStack() const { return m_LayerStack; }

		[[nodiscard]] ExportRegion& GetExportRegion() { return m_ExportRegion; }
		[[nodiscard]] const ExportRegion& GetExportRegion() const { return m_ExportRegion; }

		void AddTextureAtlas(std::shared_ptr<TextureAtlas> atlas);
		[[nodiscard]] std::shared_ptr<TextureAtlas> GetTextureAtlas(size_t index);

		// The atlas with the given stable id, or null if none matches (including
		// AtlasId::Invalid). Unlike GetTextureAtlas, this survives reorder/remove.
		[[nodiscard]] std::shared_ptr<TextureAtlas> GetTextureAtlasById(AtlasId id);

		void RemoveTextureAtlas(size_t index);
		void ClearTextureAtlases();
		[[nodiscard]] size_t GetTextureAtlasCount() const { return m_TextureAtlases.size(); }

		// The next id AddTextureAtlas will mint. Persisted so a reloaded project
		// keeps allocating fresh ids past any that removes have retired.
		[[nodiscard]] uint32_t GetNextAtlasId() const { return m_NextAtlasId; }
		void SetNextAtlasId(uint32_t next) { m_NextAtlasId = next; }

	private:
		std::string m_ProjectName = "New Project";
		std::filesystem::path m_FilePath;               // empty for a never-saved project
		std::chrono::steady_clock::time_point m_LastAccessed;
		std::chrono::steady_clock::time_point m_LastSaved;
		bool m_HasUnsavedChanges = false;
		std::vector<std::shared_ptr<TextureAtlas>> m_TextureAtlases;
		uint32_t m_NextAtlasId = 1;                     // monotonic; 0 is the invalid id
		LayerStack m_LayerStack;
		ExportRegion m_ExportRegion;                    // export/canvas region, tile space
	};
}
