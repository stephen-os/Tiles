#include "PanelMenuBar.h"
#include "Core/Constants.h"
#include "ImGuiFileDialog.h"
#include <filesystem>

#include "Lumina/Core/Application.h"

namespace Tiles
{
    PanelMenuBar::PanelMenuBar(Ref<Context> context) : Panel(context), 
		m_PopupSaveAs(context), m_PopupOpenProject(context)
    {
    }

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
            if (m_Context && m_Context->HasProject())
            {
                ImGui::Separator();
                ImGui::TextColored(UI::Color::Text, "%s", m_Context->GetProjectDisplayName().c_str());

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

        m_PopupSaveAs.Render();
        m_PopupOpenProject.Render();

        if (m_PopupRenderMatrix.IsVisible())
        {
            m_PopupRenderMatrix.Render();
        }
        
    }

    void PanelMenuBar::Update()
    {
        HandleKeyboardShortcuts();
    }

    void PanelMenuBar::HandleKeyboardShortcuts()
    {
        if (!m_Context) return;

        ImGuiIO& io = ImGui::GetIO();

        if (io.KeyCtrl)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_N, false))
            {
                m_ShowNewProjectDialog = true;
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_O, false))
            {
				m_PopupOpenProject.Toggle(); 
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_S, false))
            {
                auto project = m_Context->GetProject();
                if (project->IsNew() && project->HasUnsavedChanges())
                {
                    m_PopupSaveAs.Toggle();
                }
                else
                {
                    m_Context->SaveProject(); 
                }
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_Z, false))
            {
                if (m_Context->CanUndo())
                {
                    m_Context->Undo();
                }
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_Y, false) ||
                (io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_Z, false)))
            {
                if (m_Context->CanRedo())
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
				m_PopupOpenProject.Toggle();
            }

            ImGui::Separator();

            bool hasProject = m_Context && m_Context->HasProject();
            if (ImGui::MenuItem("Save", "Ctrl+S", false, hasProject))
            {
                auto project = m_Context->GetProject();
                if (project->IsNew() && project->HasUnsavedChanges())
                {
                    m_PopupSaveAs.Show();
                }
                else
                {
                    m_Context->SaveProject();
                }
            }

            if (ImGui::MenuItem("Save As", "Ctrl+Shift+S", false, hasProject))
            {
				m_PopupSaveAs.Show();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Export...", "Ctrl+E", false, hasProject))
            {
                ShowExportDialog();
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
                if (m_Context)
                {
					// Application::GetInstance().Shutdown();
                }
            }

            ImGui::EndMenu();
        }
    }

    void PanelMenuBar::RenderEditMenu()
    {
        if (ImGui::BeginMenu("Edit"))
        {
            bool hasProject = m_Context && m_Context->HasProject();
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
            bool hasProject = m_Context && m_Context->HasProject();

            if (ImGui::MenuItem("Resize Project", nullptr, false, hasProject))
            {
                if (hasProject)
                {
                    // Initialize resize dialog with current dimensions
                    const auto& layerStack = m_Context->GetProject()->GetLayerStack();
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
                    const auto& layerStack = project->GetLayerStack();

                    ImGui::Text("Name: %s", project->GetProjectName().c_str());
                    ImGui::Text("Dimensions: %dx%d", layerStack.GetWidth(), layerStack.GetHeight());
                    ImGui::Text("Layers: %zu", layerStack.GetLayerCount());
                    ImGui::Text("Atlases: %zu", project->GetTextureAtlasCount());

                    if (project->HasUnsavedChanges())
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
                ImGui::MenuItem("Debug Panel", nullptr, true);
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

    void PanelMenuBar::CreateNewProject()
    {
        if (!m_Context)
            return;

        m_Context->CreateProject(
            std::string(m_NewProjectName),
            static_cast<uint32_t>(m_NewProjectWidth),
            static_cast<uint32_t>(m_NewProjectHeight)
        );
    }

    void PanelMenuBar::ResizeCurrentProject()
    {
        if (!m_Context || !m_Context->HasProject())
            return;

        m_Context->ResizeProject(
            static_cast<uint32_t>(m_ResizeWidth),
            static_cast<uint32_t>(m_ResizeHeight)
        );
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
                CreateNewProject();
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
                ResizeCurrentProject();
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

    void PanelMenuBar::ShowFileDialog()
    {
        if (m_FileDialogMode == FileDialogMode::Open)
        {
            // Using ImGuiFileDialog library
            if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
                    if (m_Context)
                    {
                        m_Context->LoadProject(filePath);
                    }
                }
                ImGuiFileDialog::Instance()->Close();
                m_ShowOpenDialog = false;
                m_FileDialogMode = FileDialogMode::None;
            }
        }
        else if (m_FileDialogMode == FileDialogMode::SaveAs)
        {
            if (ImGuiFileDialog::Instance()->Display("SaveFileDlgKey"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
                    if (m_Context)
                    {
                        m_Context->SaveProjectAs(filePath);
                    }
                }
                ImGuiFileDialog::Instance()->Close();
                m_ShowSaveAsDialog = false;
                m_FileDialogMode = FileDialogMode::None;
            }
        }

        // Initialize file dialogs when needed
        if (m_ShowOpenDialog && m_FileDialogMode == FileDialogMode::Open)
        {
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Open Project", ".tiles", config);
            m_ShowOpenDialog = false; // Only show once
        }

        if (m_ShowSaveAsDialog && m_FileDialogMode == FileDialogMode::SaveAs)
        {
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("SaveFileDlgKey", "Save Project As", ".tiles", config);
            m_ShowSaveAsDialog = false; // Only show once
        }
    }

    void PanelMenuBar::ShowExportDialog()
    {
        if (m_Context && m_Context->HasProject())
        {
            m_PopupRenderMatrix.Show(m_Context);
        }
    }
}