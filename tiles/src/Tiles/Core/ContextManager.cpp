#include "ContextManager.h"

#include "Lumina/Core/Assert.h"
#include "Lumina/Core/Log.h"

namespace Tiles
{
    size_t ContextManager::CreateContext(uint32_t width, uint32_t height, const std::string& name)
    {
        auto project = std::make_shared<Project>(width, height, name);
        return CreateContextFromProject(project);
    }

    size_t ContextManager::CreateContextFromProject(std::shared_ptr<Project> project)
    {
        auto context = std::make_unique<Context>(project);
        m_Contexts.push_back(std::move(context));

        size_t newIndex = m_Contexts.size() - 1;
        if (!HasActiveContext())
        {
            m_ActiveContext = newIndex;
        }

        return newIndex;
    }

    size_t ContextManager::LoadProjectContext(const std::string& filepath)
    {
		auto project = std::make_shared<Project>(0, 0);
        return CreateContextFromProject(project->Deserialize(filepath));
    }

    void ContextManager::CloseContext(size_t index)
    {
        LUMINA_ASSERT(IsValidContextIndex(index), "Invalid context index for closing");

        m_Contexts.erase(m_Contexts.begin() + index);
        ValidateActiveContext();
    }

    void ContextManager::CloseAllContexts()
    {
        m_Contexts.clear();
        m_ActiveContext = 0;
    }

    void ContextManager::SetActiveContext(size_t index)
    {
        LUMINA_ASSERT(IsValidContextIndex(index), "Invalid context index");
        m_ActiveContext = index;
        GetActiveContext().UpdateLastAccessed();
    }

    bool ContextManager::HasActiveContext() const
    {
        return IsValidContextIndex(m_ActiveContext);
    }

    Context& ContextManager::GetActiveContext()
    {
        LUMINA_ASSERT(HasActiveContext(), "No active context");
        return *m_Contexts[m_ActiveContext];
    }

    const Context& ContextManager::GetActiveContext() const
    {
        LUMINA_ASSERT(HasActiveContext(), "No active context");
        return *m_Contexts[m_ActiveContext];
    }

    Context& ContextManager::GetContext(size_t index)
    {
        LUMINA_ASSERT(IsValidContextIndex(index), "Invalid context index");
        return *m_Contexts[index];
    }

    const Context& ContextManager::GetContext(size_t index) const
    {
        LUMINA_ASSERT(IsValidContextIndex(index), "Invalid context index");
        return *m_Contexts[index];
    }

    bool ContextManager::SaveActiveProject()
    {
        if (!HasActiveContext())
            return false;

        Context& context = GetActiveContext();
        Project& project = context.GetProject();

        if (project.IsNew())
        {
            LUMINA_LOG_WARN("Cannot save new project without filepath. Use SaveAs instead.");
            return false;
        }

        return true; // project.Save(project.GetFilePath());
    }

    bool ContextManager::SaveActiveProjectAs(const std::string& filepath)
    {
        if (!HasActiveContext())
            return false;

        Context& context = GetActiveContext();
        return true; // context.GetProject().SaveAs(filepath);
    }

    std::vector<std::string> ContextManager::GetProjectNames() const
    {
        std::vector<std::string> names;
        names.reserve(m_Contexts.size());

        for (const auto& context : m_Contexts)
        {
            names.push_back(context->GetProject().GetProjectName());
        }

        return names;
    }

    void ContextManager::ValidateActiveContext()
    {
        if (!HasActiveContext() && !IsEmpty())
        {
            m_ActiveContext = 0;
        }
        else if (IsEmpty())
        {
            m_ActiveContext = 0;
        }
        else if (m_ActiveContext >= m_Contexts.size())
        {
            m_ActiveContext = m_Contexts.size() - 1;
        }
    }
}