#include "Domain/Project.h"

#include "Core/Logger.h"
#include "Core/Assert.h"

#include "Domain/Constants.h"

namespace Tiles
{
	// Creates a named project with one empty starter layer.
	Project::Project(const std::string& name) : m_ProjectName(name)
	{
		TILES_ENGINE_INFO("Project::Project: Creating new project: '{}'", name);

		// The board is sparse and unbounded; a fresh project still needs one layer
		// to paint on.
		m_LayerStack.AddLayer("Layer 1");
		UpdateLastAccessed();
	}

	// Renames the project, ignoring an empty name and marking it modified on change.
	void Project::SetProjectName(const std::string& name)
	{
		if (name.empty())
		{
			TILES_ENGINE_INFO("Project::SetProjectName: Attempted to set empty project name - ignoring");
			return;
		}

		if (name != m_ProjectName)
		{
			TILES_ENGINE_INFO("Project::SetProjectName: Project name changed from '{}' to '{}'", m_ProjectName, name);
			m_ProjectName = name;
			MarkAsModified();
		}
	}

	// Appends an atlas, ignoring null.
	void Project::AddTextureAtlas(std::shared_ptr<TextureAtlas> atlas)
	{
		if (!atlas)
		{
			TILES_ENGINE_INFO("Project::AddTextureAtlas: Attempted to add null texture atlas - ignoring");
			return;
		}

		atlas->SetId(static_cast<AtlasId>(m_NextAtlasId++));
		m_TextureAtlases.push_back(std::move(atlas));
		TILES_ENGINE_INFO("Project::AddTextureAtlas: Added texture atlas (total count: {})", m_TextureAtlases.size());
		MarkAsModified();
	}

	// The atlas at index; asserts the index is valid.
	std::shared_ptr<TextureAtlas> Project::GetTextureAtlas(size_t index)
	{
		TILES_ASSERT(index < m_TextureAtlases.size(), "Project::GetTextureAtlas: Index {} out of bounds (size: {})", index, m_TextureAtlases.size());
		return m_TextureAtlases[index];
	}

	// The atlas with the given stable id, or null if none matches.
	std::shared_ptr<TextureAtlas> Project::GetTextureAtlasById(AtlasId id)
	{
		if (id == AtlasId::Invalid)
			return nullptr;

		for (const auto& atlas : m_TextureAtlases)
			if (atlas->GetId() == id)
				return atlas;

		return nullptr;
	}

	// Removes the atlas at index; a no-op for an invalid index.
	void Project::RemoveTextureAtlas(size_t index)
	{
		if (index >= m_TextureAtlases.size())
		{
			TILES_ENGINE_INFO("Project::RemoveTextureAtlas: Attempted to remove texture atlas at invalid index {} (size: {})", index, m_TextureAtlases.size());
			return;
		}

		m_TextureAtlases.erase(m_TextureAtlases.begin() + index);
		TILES_ENGINE_INFO("Project::RemoveTextureAtlas: Removed texture atlas at index {} (remaining count: {})", index, m_TextureAtlases.size());
		MarkAsModified();
	}

	// Removes every atlas.
	void Project::ClearTextureAtlases()
	{
		size_t previousCount = m_TextureAtlases.size();
		m_TextureAtlases.clear();
		TILES_ENGINE_INFO("Project::ClearTextureAtlases: Cleared {} texture atlases", previousCount);
		MarkAsModified();
	}

	// Reconstructs a project from the legacy path-referencing JSON format, loading
	// each atlas's image from its stored path.
	std::shared_ptr<Project> Project::FromJSON(const nlohmann::json& json)
	{
		TILES_ENGINE_INFO("Project::FromJSON: Loading project from JSON");

		if (!json.contains(JSON::Project::LayerStack))
		{
			TILES_ENGINE_INFO("Project::FromJSON: JSON missing required LayerStack field");
			return nullptr;
		}

		LayerStack layerStack = LayerStack::FromJSON(json.at(JSON::Project::LayerStack));
		std::string projectName = json.value(JSON::Project::Name, "Loaded Project");

		TILES_ENGINE_INFO("Project::FromJSON: Creating project '{}'", projectName);

		std::shared_ptr<Project> project = std::make_shared<Project>(projectName);
		project->m_LayerStack = std::move(layerStack);

		if (json.contains(JSON::Atlas::Array))
		{
			const auto& atlasesArray = json[JSON::Atlas::Array];
			project->m_TextureAtlases.reserve(atlasesArray.size());

			size_t loadedCount = 0;
			uint32_t position = 0;
			for (const auto& jsonAtlas : atlasesArray)
			{
				// The 1-based array position is the atlas's stable id: the legacy
				// migration maps each tile's old positional index to this same id,
				// so the assignment survives skipped (invalid) entries.
				++position;

				std::string texturePath = jsonAtlas.value(JSON::Atlas::Path, "");
				uint32_t atlasWidth = jsonAtlas.value(JSON::Atlas::Width, 0);
				uint32_t atlasHeight = jsonAtlas.value(JSON::Atlas::Height, 0);

				if (texturePath.empty() || atlasWidth == 0 || atlasHeight == 0)
				{
					TILES_ENGINE_INFO("Project::FromJSON: Skipping invalid atlas entry: path='{}', width={}, height={}", texturePath, atlasWidth, atlasHeight);
					continue;
				}

				auto atlas = TextureAtlas::Create(texturePath, atlasWidth, atlasHeight);
				atlas->SetId(static_cast<AtlasId>(position));
				project->m_TextureAtlases.push_back(std::move(atlas));
				loadedCount++;
			}
			project->m_NextAtlasId = static_cast<uint32_t>(atlasesArray.size()) + 1;

			TILES_ENGINE_INFO("Project::FromJSON: Loaded {} texture atlases (attempted {})", loadedCount, atlasesArray.size());
		}

		TILES_ENGINE_INFO("Project::FromJSON: Project '{}' loaded successfully", projectName);
		return project;
	}
}
