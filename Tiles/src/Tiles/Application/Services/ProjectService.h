#pragma once

#include "../../Domain/Entities/TileProject.h"
#include "../../Domain/Interfaces/IProjectRepository.h"

#include <memory>
#include <functional>
#include <filesystem>

namespace Tiles::Services
{
    // Result type for service operations
    struct ServiceResult
    {
        bool Success = false;
        std::string Message;

        static ServiceResult Ok(const std::string& message = "") { return { true, message }; }
        static ServiceResult Error(const std::string& message) { return { false, message }; }

        operator bool() const { return Success; }
    };

    // Application service for project management
    // Orchestrates domain operations and repository calls
    class ProjectService
    {
    public:
        using ProjectChangedCallback = std::function<void(Domain::TileProject*)>;

        explicit ProjectService(std::shared_ptr<Domain::IProjectRepository> repository);
        ~ProjectService() = default;

        // Project lifecycle
        void CreateNew(uint32_t width, uint32_t height, const std::string& name = "Untitled");
        ServiceResult Save();
        ServiceResult SaveAs(const std::filesystem::path& path);
        ServiceResult Load(const std::filesystem::path& path);
        void Close();

        // Project access
        Domain::TileProject* GetCurrentProject() { return m_CurrentProject.get(); }
        const Domain::TileProject* GetCurrentProject() const { return m_CurrentProject.get(); }
        bool HasProject() const { return m_CurrentProject != nullptr; }

        // Callbacks
        void SetProjectChangedCallback(ProjectChangedCallback callback) { m_OnProjectChanged = std::move(callback); }

    private:
        void NotifyProjectChanged();

        std::shared_ptr<Domain::IProjectRepository> m_Repository;
        std::unique_ptr<Domain::TileProject> m_CurrentProject;
        ProjectChangedCallback m_OnProjectChanged;
    };
}
