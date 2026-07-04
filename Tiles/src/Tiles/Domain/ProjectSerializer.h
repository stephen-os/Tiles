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

    /// Reads and writes Project documents as a self-contained container: a ZIP
    /// holding a JSON manifest plus the atlas images embedded as PNGs, so a saved
    /// project never depends on the original image paths. Legacy path-referencing
    /// JSON projects still load (detected by content). Pure I/O: it owns no editing
    /// state, history, or camera concerns, so the same routines back Save/Save-As.
    class ProjectSerializer
    {
    public:
        /// Writes project to path as a container, creating any missing parent
        /// directories. Requires a current GL context (atlases are read back from
        /// the GPU to embed).
        /// @return Failure with a reason if encoding or writing fails.
        static ProjectResult Save(const Project& project, const std::filesystem::path& path);

        /// Loads a project from path -- a container, or a legacy JSON file.
        /// @param outProject Receives the loaded project on success; left untouched otherwise.
        /// @return Failure with a reason if the file cannot be opened or parsed.
        static ProjectResult Load(const std::filesystem::path& path, std::shared_ptr<Project>& outProject);
    };
}
