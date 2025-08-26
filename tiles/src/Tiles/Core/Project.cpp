#include "Project.h"

#include "Lumina/Core/Log.h"
#include "Lumina/Core/Assert.h"

#include "Constants.h"

#include <filesystem>
#include <fstream>

namespace Tiles
{
    Project::Project(uint32_t width, uint32_t height, const std::string& name)
        : m_ProjectName(name), m_LayerStack(width, height), m_HasUnsavedChanges(false)
    {
        UpdateLastAccessed();

        m_LayerStack.AddLayer("Background");
    }

    void Project::AddTextureAtlas(Ref<Lumina::TextureAtlas> atlas)
    {
        if (atlas)
        {
            m_TextureAtlases.push_back(atlas);
            MarkAsModified();
        }
    }

    Ref<Lumina::TextureAtlas> Project::GetTextureAtlas(size_t index)
    {
        LUMINA_ASSERT(index >= 0 && index < m_TextureAtlases.size(), "Project::GetTextureAtas: Index out of bounds.");
        return m_TextureAtlases[index];
    }

    void Project::RemoveTextureAtlas(size_t index)
    {
        if (index < m_TextureAtlases.size())
        {
            m_TextureAtlases.erase(m_TextureAtlases.begin() + index);
            MarkAsModified();
        }
    }

    void Project::ClearTextureAtlases()
    {
        m_TextureAtlases.clear();
        MarkAsModified();
    }

    nlohmann::json Project::ToJSON() const
    {
        nlohmann::json jsonProject;

        jsonProject[JSON::Project::Name] = GetProjectName();
        jsonProject[JSON::Project::LayerStack] = GetLayerStack().ToJSON();

        // Serialize texture atlases
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

        return jsonProject;
    }

    Ref<Project> Project::FromJSON(const nlohmann::json& json)
    {
        LayerStack layerStack = LayerStack::FromJSON(json.at(JSON::Project::LayerStack));

        std::string projectName = json.value(JSON::Project::Name, "Loaded Project");

        Ref<Project> project = CreateRef<Project>(layerStack.GetWidth(), layerStack.GetHeight(), projectName);

        project->m_LayerStack = layerStack;

        if (json.contains(JSON::Atlas::Array))
        {
            const auto& atlasesArray = json[JSON::Atlas::Array];

            project->m_TextureAtlases.reserve(atlasesArray.size());

            for (const auto& jsonAtlas : atlasesArray)
            {
                std::string texturePath = jsonAtlas.value(JSON::Atlas::Path, "");
                uint32_t atlasWidth = jsonAtlas.value(JSON::Atlas::Width, 0);
                uint32_t atlasHeight = jsonAtlas.value(JSON::Atlas::Height, 0);

                auto atlas = Lumina::TextureAtlas::Create(texturePath, atlasWidth, atlasHeight);
                project->m_TextureAtlases.push_back(atlas);
            }
        }

        return project;
    }
}