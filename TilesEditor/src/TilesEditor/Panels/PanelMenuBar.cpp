#include "PanelMenuBar.h"
#include "../UI/Theme.h"

#include "Core/Application.h"
#include "Session/Workspace.h"

namespace Tiles::Editor
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
            if (Ctx().HasProject())
            {
                ImGui::Separator();
                ImGui::TextColored(UI::GetTheme().Text, "%s", Ctx().GetProjectDisplayName().c_str());

                if (Ctx().IsDirty())
                {
                    ImGui::SameLine();
                    ImGui::TextColored(UI::GetTheme().Danger, "*");
                }
            }

            ImGui::EndMainMenuBar();
        }
    }

    void PanelMenuBar::RenderFileMenu()
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Project", Host().ShortcutLabel(ActionId::NewProject).c_str()))
                Host().Invoke(ActionId::NewProject);

            if (ImGui::MenuItem("Open Project", Host().ShortcutLabel(ActionId::OpenProject).c_str()))
                Host().Invoke(ActionId::OpenProject);

            ImGui::Separator();

            bool hasProject = Ctx().HasProject();
            if (ImGui::MenuItem("Save", Host().ShortcutLabel(ActionId::Save).c_str(), false, hasProject))
                Host().Invoke(ActionId::Save);

            if (ImGui::MenuItem("Save As", Host().ShortcutLabel(ActionId::SaveAs).c_str(), false, hasProject))
                Host().Invoke(ActionId::SaveAs);

            ImGui::Separator();

            if (ImGui::MenuItem("Export...", Host().ShortcutLabel(ActionId::Export).c_str(), false, hasProject))
                Host().Invoke(ActionId::Export);

            ImGui::Separator();

            bool hasRecentProjects = Space().HasRecentProjects();

            if (ImGui::BeginMenu("Recent Projects", hasRecentProjects))
            {
                if (hasRecentProjects)
                {
                    size_t projectCount = Space().GetRecentProjectCount();
                    for (size_t i = 0; i < projectCount; ++i)
                    {
                        const auto& entry = Space().GetRecentProject(i);

                        std::string menuText = entry.displayName;
                        std::string shortcut = "";
                        if (i < 9)
                        {
                            shortcut = "Ctrl+" + std::to_string(i + 1);
                        }

                        if (ImGui::MenuItem(menuText.c_str(), shortcut.empty() ? nullptr : shortcut.c_str()))
                        {
                            auto result = Space().LoadProject(entry.filePath);
                            if (!result)
                                Host().Notify(result.error().message);
                        }

                        if (ImGui::IsItemHovered())
                        {
                            ImGui::SetTooltip("%s", entry.filePath.string().c_str());
                        }
                    }

                    ImGui::Separator();
                    if (ImGui::MenuItem("Clear Recent Projects"))
                    {
                        Space().ClearRecentProjects();
                    }
                }
                else
                {
                    ImGui::MenuItem("No recent projects", nullptr, false, false);
                }
                ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                Tiles::Application::GetInstance().Shutdown();
            }

            ImGui::EndMenu();
        }
    }

    void PanelMenuBar::RenderEditMenu()
    {
        if (ImGui::BeginMenu("Edit"))
        {
            bool hasProject = Ctx().HasProject();
            bool canUndo = hasProject && Ctx().CanUndo();
            bool canRedo = hasProject && Ctx().CanRedo();

            if (ImGui::MenuItem("Undo", Host().ShortcutLabel(ActionId::Undo).c_str(), false, canUndo))
                Host().Invoke(ActionId::Undo);

            if (ImGui::MenuItem("Redo", Host().ShortcutLabel(ActionId::Redo).c_str(), false, canRedo))
                Host().Invoke(ActionId::Redo);

            ImGui::Separator();

            if (ImGui::MenuItem("Clear History", nullptr, false, hasProject))
                Host().Invoke(ActionId::ClearHistory);

            ImGui::EndMenu();
        }
    }

    void PanelMenuBar::RenderProjectMenu()
    {
        if (ImGui::BeginMenu("Project"))
        {
            bool hasProject = Ctx().HasProject();

            if (ImGui::BeginMenu("Project Info", hasProject))
            {
                if (hasProject)
                {
                    const auto& project = Ctx().GetProject();
                    const auto& layerStack = project->GetLayerStack();

                    ImGui::Text("Name: %s", project->GetProjectName().c_str());
                    ImGui::Text("Layers: %zu", layerStack.GetLayerCount());
                    ImGui::Text("Atlases: %zu", project->GetTextureAtlasCount());

                    if (project->HasUnsavedChanges())
                    {
                        ImGui::TextColored(UI::GetTheme().Danger, "Unsaved Changes");
                    }
                    else
                    {
                        ImGui::TextColored(UI::GetTheme().Success, "Saved");
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
            if (ImGui::BeginMenu("Panels"))
            {
                // Every registered panel (bar the menu bar) shows up here as a
                // checkable toggle, driven straight off its open state.
                for (const auto& [id, title] : Host().ToggleablePanels())
                {
                    if (ImGui::MenuItem(title.c_str(), nullptr, Host().IsPanelOpen(id)))
                        Host().TogglePanel(id);
                }
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
                Host().OpenPopup(PopupId::About);
            }

            ImGui::EndMenu();
        }
    }
}
