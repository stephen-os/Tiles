#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Context.h"

namespace Tiles
{
    class ContextManager
    {
    public:
        ContextManager() = default;
        ~ContextManager() = default;

        // Context management
        size_t CreateContext(uint32_t width, uint32_t height, const std::string& name = "Untitled Project");
        size_t CreateContextFromProject(std::shared_ptr<Project> project);
        size_t LoadProjectContext(const std::string& filepath);
        void CloseContext(size_t index);
        void CloseAllContexts();

        // Active context management
        void SetActiveContext(size_t index);
        size_t GetActiveContextIndex() const { return m_ActiveContext; }
        bool HasActiveContext() const;

        // Context access
        Context& GetActiveContext();
        const Context& GetActiveContext() const;
        Context& GetContext(size_t index);
        const Context& GetContext(size_t index) const;

        // Project operations through active context
        bool SaveActiveProject();
        bool SaveActiveProjectAs(const std::string& filepath);

        // Utility
        size_t GetContextCount() const { return m_Contexts.size(); }
        bool IsEmpty() const { return m_Contexts.empty(); }
        bool IsValidContextIndex(size_t index) const { return index < m_Contexts.size(); }

        // Get project names for UI
        std::vector<std::string> GetProjectNames() const;

    private:
        void ValidateActiveContext();

        std::vector<std::unique_ptr<Context>> m_Contexts;
        size_t m_ActiveContext = 0;
    };
}