#pragma once

#include "../../Domain/Interfaces/IProjectRepository.h"

namespace Tiles::Infrastructure
{
    // JSON-based implementation of IProjectRepository
    // Handles serialization/deserialization of projects to .tiles files
    class JsonProjectRepository : public Domain::IProjectRepository
    {
    public:
        JsonProjectRepository() = default;
        ~JsonProjectRepository() override = default;

        Domain::RepositoryResult<void> Save(
            const Domain::TileProject& project,
            const std::filesystem::path& path
        ) override;

        Domain::RepositoryResult<std::unique_ptr<Domain::TileProject>> Load(
            const std::filesystem::path& path
        ) override;

        bool Exists(const std::filesystem::path& path) const override;

        std::optional<std::filesystem::file_time_type> GetLastModified(
            const std::filesystem::path& path
        ) const override;

    private:
        static constexpr const char* FILE_EXTENSION = ".tiles";
        static constexpr int CURRENT_VERSION = 1;
    };
}
