#include "Editor.h"

#include "imgui.h"

#include "Panels/PanelLayerSelection.h"
#include "Panels/PanelToolSelection.h"
#include "Panels/PanelTextureSelection.h"
#include "Panels/PanelBrushPreview.h"
#include "Panels/PanelBrushAttributes.h"
#include "Panels/PanelViewport.h"

#include "Lumina/Core/Log.h"

namespace Tiles
{
    void Editor::OnAttach()
    {
        // Create a default project to start with
        m_ContextManager.CreateContext(16, 16, "Default Project");

        // Set the panel context 
        Context* activeContext = nullptr;
        if (m_ContextManager.HasActiveContext())
        {
            activeContext = &m_ContextManager.GetActiveContext();
        }

        m_PanelManager.SetContext(activeContext);

        // Initialize panels
        m_PanelManager.RegisterPanel<PanelLayerSelection>();
        m_PanelManager.RegisterPanel<PanelToolSelection>();
        m_PanelManager.RegisterPanel<PanelTextureSelection>(); 
        m_PanelManager.RegisterPanel<PanelBrushPreview>(); 
		m_PanelManager.RegisterPanel<PanelBrushAttributes>();
        m_PanelManager.RegisterPanel<PanelViewport>();
    }

    void Editor::OnDetach()
    {
        m_PanelManager.Clear();
        m_ContextManager.CloseAllContexts();
    }

    void Editor::OnUpdate(float timestep)
    {
        // Calculate FPS
        float elapsedTime = m_FrameTimer.Elapsed();
        m_FPS = 1.0f / elapsedTime;
        m_FrameTimer.Reset();

        // Get active context (or nullptr if none)
        Context* activeContext = nullptr;
        if (m_ContextManager.HasActiveContext())
        {
            activeContext = &m_ContextManager.GetActiveContext();
        }

        m_PanelManager.SetContext(activeContext);
        m_PanelManager.Update(); 
    }

    void Editor::OnUIRender()
    {
#ifdef LUMINA_DEBUG
        // Debug panels
        ImGui::Begin("FPS Counter");
        ImGui::Text("FPS: %.1f", m_FPS);
        ImGui::End();

        ImGui::Begin("Project Manager Debug");
        ImGui::Text("Active Contexts: %zu", m_ContextManager.GetContextCount());
        if (m_ContextManager.HasActiveContext())
        {
            const Context& context = m_ContextManager.GetActiveContext();
            const Project& project = context.GetProject();
            ImGui::Text("Active Project: %s", project.GetProjectName().c_str());
            ImGui::Text("Working Layer: %zu", context.GetWorkingLayer());
            ImGui::Text("Has Unsaved Changes: %s", project.HasUnsavedChanges() ? "Yes" : "No");
        }
        else
        {
            ImGui::Text("No active context");
        }
        ImGui::End();
#endif

        m_PanelManager.Render();
    }
}