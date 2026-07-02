#pragma once

#include <filesystem>
#include <memory>
#include <string>

namespace Tiles
{
    class Project;

    /// Outcome of a project file operation. Message carries a user-facing
    /// reason when Success is false.
    struct ProjectResult
    {
        bool Success = false;
        std::string Message;
    };

    /// Reads and writes Project documents as JSON files on disk. Pure I/O: it
    /// owns no editing state, history, or camera concerns, so the same routines
    /// back both Save and Save-As.
    class ProjectSerializer
    {
    public:
        /// Serializes project to path, creating any missing parent directories.
        /// @return Failure with a reason if the directory, file, or JSON dump fails.
        static ProjectResult Save(const Project& project, const std::filesystem::path& path);

        /// Reads and parses a project from path.
        /// @param outProject Receives the loaded project on success; left untouched otherwise.
        /// @return Failure with a reason if the file cannot be opened or parsed.
        static ProjectResult Load(const std::filesystem::path& path, std::shared_ptr<Project>& outProject);
    };
}
