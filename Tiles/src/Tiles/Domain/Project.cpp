#include "Domain/Project.h"

#include "Core/Logger.h"
#include "Core/Assert.h"

#include "Core/Constants.h"

#include <filesystem>
#include <fstream>

namespace Tiles
{
    Project::Project(const std::string& name) : m_ProjectName(name)
    {
        TILES_ENGINE_INFO("Project::Project: Creating new project: '{}'", name);

        // The board is sparse and unbounded; a fresh project still needs one layer
        // to paint on.
        m_LayerStack.AddLayer("Layer 1");
        UpdateLastAccessed();
    }

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

    void Project::AddTextureAtlas(std::shared_ptr<Tiles::TextureAtlas> atlas)
    {
        if (!atlas)
        {
            TILES_ENGINE_INFO("Project::AddTextureAtlas: Attempted to add null texture atlas - ignoring");
            return;
        }

        m_TextureAtlases.push_back(atlas);
        TILES_ENGINE_INFO("Project::AddTextureAtlas: Added texture atlas (total count: {})", m_TextureAtlases.size());
        MarkAsModified();
    }

    std::shared_ptr<Tiles::TextureAtlas> Project::GetTextureAtlas(size_t index)
    {
        TILES_ASSERT(index < m_TextureAtlases.size(), "Project::GetTextureAtlas: Index {} out of bounds (size: {})", index, m_TextureAtlases.size());
        return m_TextureAtlases[index];
    }

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

    void Project::ClearTextureAtlases()
    {
        size_t previousCount = m_TextureAtlases.size();
        m_TextureAtlases.clear();
        TILES_ENGINE_INFO("Project::ClearTextureAtlases: Cleared {} texture atlases", previousCount);
        MarkAsModified();
    }

    nlohmann::json Project::ToJSON() const
    {
        TILES_ENGINE_INFO("Project::ToJSON: Serializing project '{}' to JSON", m_ProjectName);

        nlohmann::json jsonProject;

        jsonProject[JSON::Project::Name] = GetProjectName();
        jsonProject[JSON::Project::LayerStack] = GetLayerStack().ToJSON();

        nlohmann::json atlasArray = nlohmann::json::array();
        for (const auto& atlas : GetTextureAtlases())
        {
            if (atlas && atlas->GetTexture())
            {
                nlohmann::json jsonAtlas;
                jsonAtlas[JSON::Atlas::Path] = atlas->GetTexture()->GetPath();
                jsonAtlas[JSON::Atlas::Width] = atlas->GetWidth();
                jsonAtlas[JSON::Atlas::Height] = atlas->GetHeight();
                atlasArray.push_back(jsonAtlas);
            }
        }
        jsonProject[JSON::Atlas::Array] = atlasArray;

        TILES_ENGINE_INFO("Project::ToJSON: Serialized project with {} texture atlases", GetTextureAtlases().size());

        return jsonProject;
    }

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
            for (const auto& jsonAtlas : atlasesArray)
            {
                std::string texturePath = jsonAtlas.value(JSON::Atlas::Path, "");
                uint32_t atlasWidth = jsonAtlas.value(JSON::Atlas::Width, 0);
                uint32_t atlasHeight = jsonAtlas.value(JSON::Atlas::Height, 0);

                if (texturePath.empty() || atlasWidth == 0 || atlasHeight == 0)
                {
                    TILES_ENGINE_INFO("Project::FromJSON: Skipping invalid atlas entry: path='{}', width={}, height={}", texturePath, atlasWidth, atlasHeight);
                    continue;
                }

                auto atlas = Tiles::TextureAtlas::Create(texturePath, atlasWidth, atlasHeight);
                if (atlas)
                {
                    project->m_TextureAtlases.push_back(atlas);
                    loadedCount++;
                }
                else
                {
                    TILES_ENGINE_INFO("Project::FromJSON: Failed to create texture atlas from path: {}", texturePath);
                }
            }

            TILES_ENGINE_INFO("Project::FromJSON: Loaded {} texture atlases (attempted {})", loadedCount, atlasesArray.size());
        }

        TILES_ENGINE_INFO("Project::FromJSON: Project '{}' loaded successfully", projectName);
        return project;
    }
}