#pragma once

#include <expected>
#include <filesystem>
#include <memory>

#include "Core/Error.h"

namespace Tiles
{
    class Project;

    // Reads and writes Project documents as a self-contained container: a ZIP
    // holding a JSON manifest plus the atlas images embedded as PNGs, so a saved
    // project never depends on the original image paths. Legacy path-referencing
    // JSON projects still load (detected by content). Pure I/O: it owns no editing
    // state, history, or camera concerns, so the same routines back Save/Save-As.
    class ProjectSerializer
    {
    public:
        // Writes project to path as a container, creating any missing parent
        // directories. Embeds each atlas's source image bytes directly, so no GL
        // context or GPU readback is involved.
        // @return A WriteFailure error if encoding or writing fails.
        [[nodiscard]] static std::expected<void, Error> Save(const Project& project, const std::filesystem::path& path);

        // Loads a project from path -- a container, or a legacy JSON file.
        // @return The loaded project, or a ReadFailure error if it cannot be
        //         opened or parsed.
        [[nodiscard]] static std::expected<std::shared_ptr<Project>, Error> Load(const std::filesystem::path& path);
    };
}
