#include "ProjectSerializer.h"

#include <fstream>

#include "json.hpp"

#include "Project.h"

namespace Tiles
{
    ProjectResult ProjectSerializer::Save(const Project& project, const std::filesystem::path& path)
    {
        auto directory = path.parent_path();
        if (!directory.empty() && !std::filesystem::exists(directory))
        {
            std::filesystem::create_directories(directory);
        }

        try
        {
            nlohmann::json projectJSON = project.ToJSON();

            std::ofstream file(path);
            if (!file.is_open())
            {
                return { false, "Failed to open file for writing." };
            }

            file << projectJSON.dump(4);
            file.close();

            return { true, "Project saved successfully." };
        }
        catch (const std::exception& e)
        {
            return { false, std::string("Failed to save project: ") + e.what() };
        }
    }

    ProjectResult ProjectSerializer::Load(const std::filesystem::path& path, std::shared_ptr<Project>& outProject)
    {
        try
        {
            std::ifstream file(path);
            if (!file.is_open())
            {
                return { false, "Failed to open file for reading." };
            }

            nlohmann::json jsonProject;
            file >> jsonProject;
            file.close();

            outProject = Project::FromJSON(jsonProject);
            if (!outProject)
            {
                // Parseable JSON that is missing required fields (e.g. no layer
                // stack) yields a null project; fail cleanly instead of letting
                // the caller dereference it.
                return { false, "Invalid project file format." };
            }

            return { true, "Project loaded successfully." };
        }
        catch (const nlohmann::json::parse_error& e)
        {
            return { false, std::string("Invalid project file format: ") + e.what() };
        }
        catch (const std::exception& e)
        {
            return { false, std::string("Failed to load project: ") + e.what() };
        }
    }
}
