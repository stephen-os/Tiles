#include "PanelMenuBar.h"
#include "Constants.h"
#include "ImGuiFileDialog.h"

namespace Tiles
{
    void PanelMenuBar::Render()
    {
        if (ImGui::BeginMainMenuBar())
        {
            RenderFileMenu();
            RenderEditMenu();
            RenderProjectMenu();
            RenderViewMenu();
            RenderHelpMenu();

            // Show project name in the menu bar
            if (IsProjectLoaded())
            {
                ImGui::Separator();
                ImGui::TextColored(UI::Color::Text, "%s", GetProjectDisplayName().c_str());

                if (m_Context->IsDirty())
                {
                    ImGui::SameLine();
                    ImGui::TextColored(UI::Color::TextError, "*");
                }
            }

            ImGui::EndMainMenuBar();
        }

        // Render dialogs
        if (m_ShowNewProjectDialog)
        {
            ShowNewProjectDialog();
        }

        if (m_ShowResizeProjectDialog)
        {
            ShowResizeProjectDialog();
        }

        if (m_ShowAboutDialog)
        {
            ShowAboutDialog();
        }
    }

    void PanelMenuBar::Update()
    {
        // Handle keyboard shortcuts
        ImGuiIO& io = ImGui::GetIO();

        if (io.KeyCtrl)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_N, false))
            {
                m_ShowNewProjectDialog = true;
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_O, false))
            {
                if (m_OpenProjectCallback) m_OpenProjectCallback();
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_S, false))
            {
                if (io.KeyShift)
                {
                    if (m_SaveAsProjectCallback) m_SaveAsProjectCallback();
                }
                else
                {
                    if (m_SaveProjectCallback) m_SaveProjectCallback();
                }
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_Z, false))
            {
                if (m_Context && m_Context->CanUndo())
                {
                    m_Context->Undo();
                }
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_Y, false) ||
                (io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_Z, false)))
            {
                if (m_Context && m_Context->CanRedo())
                {
                    m_Context->Redo();
                }
            }
        }
    }

    void PanelMenuBar::RenderFileMenu()
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Project", "Ctrl+N"))
            {
                m_ShowNewProjectDialog = true;
            }

            if (ImGui::MenuItem("Open Project", "Ctrl+O"))
            {
                if (m_OpenProjectCallback) m_OpenProjectCallback();
            }

            ImGui::Separator();

            bool hasProject = IsProjectLoaded();
            if (ImGui::MenuItem("Save", "Ctrl+S", false, hasProject))
            {
                if (m_SaveProjectCallback) m_SaveProjectCallback();
            }

            if (ImGui::MenuItem("Save As", "Ctrl+Shift+S", false, hasProject))
            {
                if (m_SaveAsProjectCallback) m_SaveAsProjectCallback();
            }

            ImGui::Separator();

            if (ImGui::BeginMenu("Recent Projects", false)) // TODO: Implement recent projects
            {
                ImGui::MenuItem("No recent projects", nullptr, false, false);
                ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                if (m_ExitCallback) m_ExitCallback();
            }

            ImGui::EndMenu();
        }
    }

    void PanelMenuBar::RenderEditMenu()
    {
        if (ImGui::BeginMenu("Edit"))
        {
            bool hasProject = IsProjectLoaded();
            bool canUndo = hasProject && m_Context->CanUndo();
            bool canRedo = hasProject && m_Context->CanRedo();

            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, canUndo))
            {
                m_Context->Undo();
            }

            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, canRedo))
            {
                m_Context->Redo();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Clear History", nullptr, false, hasProject))
            {
                m_Context->ClearHistory();
            }

            ImGui::EndMenu();
        }
    }

    void PanelMenuBar::RenderProjectMenu()
    {
        if (ImGui::BeginMenu("Project"))
        {
            bool hasProject = IsProjectLoaded();

            if (ImGui::MenuItem("Resize Project", nullptr, false, hasProject))
            {
                if (hasProject)
                {
                    // Initialize resize dialog with current dimensions
                    const auto& layerStack = m_Context->GetProject().GetLayerStack();
                    m_ResizeWidth = layerStack.GetWidth();
                    m_ResizeHeight = layerStack.GetHeight();
                    m_ShowResizeProjectDialog = true;
                }
            }

            ImGui::Separator();

            if (ImGui::BeginMenu("Project Info", hasProject))
            {
                if (hasProject)
                {
                    const auto& project = m_Context->GetProject();
                    const auto& layerStack = project.GetLayerStack();

                    ImGui::Text("Name: %s", project.GetProjectName().c_str());
                    ImGui::Text("Dimensions: %dx%d", layerStack.GetWidth(), layerStack.GetHeight());
                    ImGui::Text("Layers: %zu", layerStack.GetLayerCount());
                    ImGui::Text("Atlases: %zu", project.GetTextureAtlasCount());

                    if (project.HasUnsavedChanges())
                    {
                        ImGui::TextColored(UI::Color::TextError, "Unsaved Changes");
                    }
                    else
                    {
                        ImGui::TextColored(UI::Color::Green, "Saved");
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
    }

    void PanelMenuBar::RenderViewMenu()
    {
        if (ImGui::BeginMenu("View"))
        {
            // TODO: Add view options like panel toggles, zoom controls, etc.
            if (ImGui::MenuItem("Reset Layout"))
            {
                // TODO: Implement layout reset
            }

            ImGui::Separator();

            if (ImGui::BeginMenu("Panels"))
            {
                // TODO: Add panel visibility toggles
                ImGui::MenuItem("Layer Selection", nullptr, true);
                ImGui::MenuItem("Texture Selection", nullptr, true);
                ImGui::MenuItem("Brush Attributes", nullptr, true);
                ImGui::MenuItem("Brush Preview", nullptr, true);
                ImGui::MenuItem("Tool Selection", nullptr, true);
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
    }

    void PanelMenuBar::RenderHelpMenu()
    {
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                m_ShowAboutDialog = true;
            }

            if (ImGui::MenuItem("Keyboard Shortcuts"))
            {
                // TODO: Show shortcuts dialog
            }

            ImGui::EndMenu();
        }
    }

    void PanelMenuBar::ShowNewProjectDialog()
    {
        ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("New Project", &m_ShowNewProjectDialog,
            ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize))
        {
            ImGui::Text("Create a new tile map project");
            ImGui::Separator();

            ImGui::Text("Project Name:");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##ProjectName", m_NewProjectName, sizeof(m_NewProjectName));

            ImGui::Spacing();

            ImGui::Text("Dimensions:");
            ImGui::SetNextItemWidth(UI::Component::InputWidth);
            ImGui::InputInt("Width##NewProject", &m_NewProjectWidth);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(UI::Component::InputWidth);
            ImGui::InputInt("Height##NewProject", &m_NewProjectHeight);

            // Clamp values
            m_NewProjectWidth = std::max(1, std::min(1024, m_NewProjectWidth));
            m_NewProjectHeight = std::max(1, std::min(1024, m_NewProjectHeight));

            ImGui::Spacing();
            ImGui::Separator();

            float buttonWidth = 80.0f;
            float spacing = ImGui::GetContentRegionAvail().x - (buttonWidth * 2 + UI::Component::SpaceBetween);

            if (ImGui::Button("Create", ImVec2(buttonWidth, 0)))
            {
                if (m_NewProjectCallback)
                {
                    // TODO: Pass project parameters to callback
                    m_NewProjectCallback();
                }
                m_ShowNewProjectDialog = false;
            }

            ImGui::SameLine(0, spacing);

            if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
            {
                m_ShowNewProjectDialog = false;
            }
        }
        ImGui::End();
    }

    void PanelMenuBar::ShowResizeProjectDialog()
    {
        ImGui::SetNextWindowSize(ImVec2(350, 150), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Resize Project", &m_ShowResizeProjectDialog,
            ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize))
        {
            ImGui::Text("Resize the current project");
            ImGui::Separator();

            ImGui::Text("New Dimensions:");
            ImGui::SetNextItemWidth(UI::Component::InputWidth);
            ImGui::InputInt("Width##ResizeProject", &m_ResizeWidth);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(UI::Component::InputWidth);
            ImGui::InputInt("Height##ResizeProject", &m_ResizeHeight);

            // Clamp values
            m_ResizeWidth = std::max(1, std::min(1024, m_ResizeWidth));
            m_ResizeHeight = std::max(1, std::min(1024, m_ResizeHeight));

            ImGui::Spacing();
            ImGui::TextColored(UI::Color::TextHint, "Warning: This may remove tiles outside new bounds");

            ImGui::Spacing();
            ImGui::Separator();

            float buttonWidth = 80.0f;
            float spacing = ImGui::GetContentRegionAvail().x - (buttonWidth * 2 + UI::Component::SpaceBetween);

            if (ImGui::Button("Resize", ImVec2(buttonWidth, 0)))
            {
                if (m_Context)
                {
                    m_Context->GetProject().GetLayerStack().Resize(m_ResizeWidth, m_ResizeHeight);
                    m_Context->GetProject().MarkAsModified();
                }
                m_ShowResizeProjectDialog = false;
            }

            ImGui::SameLine(0, spacing);

            if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
            {
                m_ShowResizeProjectDialog = false;
            }
        }
        ImGui::End();
    }

    void PanelMenuBar::ShowAboutDialog()
    {
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("About Tiles", &m_ShowAboutDialog,
            ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize))
        {
            ImGui::Text("Tiles Editor");
            ImGui::Separator();

            ImGui::Text("Version: 1.0.0");
            ImGui::Text("A tile map editor built with Lumina Engine");

            ImGui::Spacing();
            ImGui::Text("Features:");
            ImGui::BulletText("Multi-layer tile editing");
            ImGui::BulletText("Texture atlas support");
            ImGui::BulletText("Brush, eraser, and fill tools");
            ImGui::BulletText("Undo/redo system");
            ImGui::BulletText("Project save/load");

            ImGui::Spacing();
            ImGui::Separator();

            float buttonWidth = 80.0f;
            float spacing = (ImGui::GetContentRegionAvail().x - buttonWidth) * 0.5f;

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + spacing);
            if (ImGui::Button("Close", ImVec2(buttonWidth, 0)))
            {
                m_ShowAboutDialog = false;
            }
        }
        ImGui::End();
    }

    bool PanelMenuBar::IsProjectLoaded() const
    {
        return m_Context != nullptr;
    }

    std::string PanelMenuBar::GetProjectDisplayName() const
    {
        if (!IsProjectLoaded()) return "";

        const auto& project = m_Context->GetProject();
        std::string name = project.GetProjectName();

        if (project.IsNew())
        {
            return name + " (Unsaved)";
        }

        return name;
    }
}