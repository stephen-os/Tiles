#include "Project.h"

#include "Lumina/Core/Log.h"
#include "Lumina/Core/Assert.h"

#include <filesystem>
#include <fstream>

namespace Tiles
{
    namespace ProjectFields
    {
        static constexpr const char* PROJECT_NAME = "project_name";
        static constexpr const char* LAYER_STACK = "layer_stack";
        static constexpr const char* TEXTURE_ATLASES = "texture_atlases";
    }

    namespace TextureAtlasFields
    {
        static constexpr const char* TEXTURE_PATH = "texture_path";
        static constexpr const char* ATLAS_WIDTH = "atlas_width";
        static constexpr const char* ATLAS_HEIGHT = "atlas_height";
    }

    Project::Project(uint32_t width, uint32_t height, const std::string& name)
        : m_ProjectName(name), m_LayerStack(width, height), m_HasUnsavedChanges(false)
    {
        UpdateLastAccessed();

        m_LayerStack.AddLayer("Background");
    }

    void Project::Serialize(const Project& project, const std::filesystem::path& filePath)
    {
        if (filePath.empty())
        {
            LUMINA_LOG_ERROR("Cannot save project without filepath");
            return;
        }

        try
        {
            nlohmann::json jsonProject = project.ToJSON();

            std::ofstream file(filePath);
            if (!file.is_open())
            {
                LUMINA_LOG_ERROR("Failed to open file for writing: {}", filePath.string());
                return;
            }

            file << jsonProject.dump(4);
            file.close();

            size_t totalTiles = project.GetLayerStack().GetLayerCount() *
                project.GetLayerStack().GetWidth() *
                project.GetLayerStack().GetHeight();

            LUMINA_LOG_INFO("[Project::Serialize] Saved project '{}' to '{}'",
                project.GetProjectName(), filePath.string());
            LUMINA_LOG_INFO("Layers: {}, Size: {}x{}, Total Tiles: {}",
                project.GetLayerStack().GetLayerCount(),
                project.GetLayerStack().GetWidth(),
                project.GetLayerStack().GetHeight(),
                totalTiles);
        }
        catch (const std::exception& e)
        {
            LUMINA_LOG_ERROR("Failed to serialize project: {}", e.what());
        }
    }

    std::unique_ptr<Project> Project::Deserialize(const std::filesystem::path& filePath)
    {
        try
        {
            std::ifstream file(filePath);
            if (!file.is_open())
            {
                LUMINA_LOG_ERROR("Failed to open file for reading: {}", filePath.string());
                return nullptr;
            }

            nlohmann::json jsonProject;
            file >> jsonProject;
            file.close();

            Project project = Project::FromJSON(jsonProject);

            project.SetFilePath(filePath.string());
            project.MarkAsSaved();
            project.UpdateLastAccessed();

            size_t totalTiles = project.GetLayerStack().GetLayerCount() * project.GetLayerStack().GetWidth() * project.GetLayerStack().GetHeight();

            LUMINA_LOG_INFO("[Project::Deserialize] Loaded project '{}' from '{}'",
                project.GetProjectName(), filePath.string());
            LUMINA_LOG_INFO("Layers: {}, Size: {}x{}, Total Tiles: {}",
                project.GetLayerStack().GetLayerCount(),
                project.GetLayerStack().GetWidth(),
                project.GetLayerStack().GetHeight(),
                totalTiles);

            return std::make_unique<Project>(std::move(project));
        }
        catch (const std::exception& e)
        {
            LUMINA_LOG_ERROR("Failed to deserialize project from '{}': {}", filePath.string(), e.what());
            return nullptr;
        }
    }

    void Project::AddTextureAtlas(Ref<TextureAtlas> atlas)
    {
        if (atlas)
        {
            m_TextureAtlases.push_back(atlas);
            MarkAsModified();
        }
    }

    Ref<TextureAtlas> Project::GetTextureAtlas(size_t index)
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

        jsonProject[ProjectFields::PROJECT_NAME] = GetProjectName();
        jsonProject[ProjectFields::LAYER_STACK] = GetLayerStack().ToJSON();  // Uses LayerStack's ToJSON method

        // Serialize texture atlases
        nlohmann::json atlasesArray = nlohmann::json::array();
        for (const auto& atlas : GetTextureAtlases())
        {
            if (atlas && atlas->GetTexture())
            {
                nlohmann::json jsonAtlas;
                jsonAtlas[TextureAtlasFields::TEXTURE_PATH] = atlas->GetTexture()->GetPath();
                jsonAtlas[TextureAtlasFields::ATLAS_WIDTH] = atlas->GetWidth();
                jsonAtlas[TextureAtlasFields::ATLAS_HEIGHT] = atlas->GetHeight();
                atlasesArray.push_back(jsonAtlas);
            }
        }
        jsonProject[ProjectFields::TEXTURE_ATLASES] = atlasesArray;

        return jsonProject;
    }

    Project Project::FromJSON(const nlohmann::json& jsonProject)
    {
        LayerStack layerStack = LayerStack::FromJSON(jsonProject.at(ProjectFields::LAYER_STACK));

        std::string projectName = jsonProject.value(ProjectFields::PROJECT_NAME, "Loaded Project");

        Project project(layerStack.GetWidth(), layerStack.GetHeight(), projectName);

        project.m_LayerStack = layerStack;

        if (jsonProject.contains(ProjectFields::TEXTURE_ATLASES))
        {
            const auto& atlasesArray = jsonProject[ProjectFields::TEXTURE_ATLASES];

            project.m_TextureAtlases.reserve(atlasesArray.size());

            for (const auto& jsonAtlas : atlasesArray)
            {
                std::string texturePath = jsonAtlas.value(TextureAtlasFields::TEXTURE_PATH, "");
                uint32_t atlasWidth = jsonAtlas.value(TextureAtlasFields::ATLAS_WIDTH, 0);
                uint32_t atlasHeight = jsonAtlas.value(TextureAtlasFields::ATLAS_HEIGHT, 0);

                auto atlas = TextureAtlas::Create(texturePath, atlasWidth, atlasHeight);
                project.m_TextureAtlases.push_back(atlas);
            }
        }

        return project;
    }
}