#include "ProjectService.h"

namespace Tiles::Services
{
    ProjectService::ProjectService(std::shared_ptr<Domain::IProjectRepository> repository)
        : m_Repository(std::move(repository))
    {
    }

    void ProjectService::CreateNew(uint32_t width, uint32_t height, const std::string& name)
    {
        m_CurrentProject = std::make_unique<Domain::TileProject>(width, height, name);
        NotifyProjectChanged();
    }

    ServiceResult ProjectService::Save()
    {
        if (!m_CurrentProject)
        {
            return ServiceResult::Error("No project is currently open");
        }

        if (m_CurrentProject->IsNew())
        {
            return ServiceResult::Error("Project has not been saved before. Use SaveAs.");
        }

        auto result = m_Repository->Save(*m_CurrentProject, m_CurrentProject->GetFilePath());
        if (!result)
        {
            return ServiceResult::Error(result.error().Message);
        }

        m_CurrentProject->MarkAsSaved();
        return ServiceResult::Ok("Project saved successfully");
    }

    ServiceResult ProjectService::SaveAs(const std::filesystem::path& path)
    {
        if (!m_CurrentProject)
        {
            return ServiceResult::Error("No project is currently open");
        }

        auto result = m_Repository->Save(*m_CurrentProject, path);
        if (!result)
        {
            return ServiceResult::Error(result.error().Message);
        }

        m_CurrentProject->SetFilePath(path);
        m_CurrentProject->MarkAsSaved();
        NotifyProjectChanged();

        return ServiceResult::Ok("Project saved successfully");
    }

    ServiceResult ProjectService::Load(const std::filesystem::path& path)
    {
        auto result = m_Repository->Load(path);
        if (!result)
        {
            return ServiceResult::Error(result.error().Message);
        }

        m_CurrentProject = std::move(result.value());
        NotifyProjectChanged();

        return ServiceResult::Ok("Project loaded successfully");
    }

    void ProjectService::Close()
    {
        m_CurrentProject.reset();
        NotifyProjectChanged();
    }

    void ProjectService::NotifyProjectChanged()
    {
        if (m_OnProjectChanged)
        {
            m_OnProjectChanged(m_CurrentProject.get());
        }
    }
}
