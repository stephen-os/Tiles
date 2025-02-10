#include "HeaderPanel.h"

#include "imgui.h"

void HeaderPanel::Render()
{
    if (ImGui::BeginMainMenuBar())
    {
        RenderFile();
        RenderEdit();
        RenderOptions();
    }
    ImGui::EndMainMenuBar();
}

void HeaderPanel::RenderFile()
{
    // File Menu
    if (ImGui::BeginMenu("File"))
    {
        // File Menu Items
        if (ImGui::MenuItem("New"))
        {
            // Placeholder for New functionality
        }
        if (ImGui::MenuItem("Save"))
        {
            // Placeholder for Save functionality
        }
        if (ImGui::MenuItem("Load"))
        {
            // Placeholder for Load functionality
        }

        ImGui::EndMenu();
    }
}

void HeaderPanel::RenderEdit()
{
    // Edit Menu
    if (ImGui::BeginMenu("Edit"))
    {
        // Edit Menu Items
        if (ImGui::MenuItem("Undo"))
        {
            // Placeholder for Undo functionality
        }
        if (ImGui::MenuItem("Redo"))
        {
            // Placeholder for Redo functionality
        }

        ImGui::EndMenu();
    }
}

void HeaderPanel::RenderOptions()
{
    // Options Menu
    if (ImGui::BeginMenu("Options"))
    {
        // Options Menu Item
        if (ImGui::MenuItem("Settings"))
        {
            // Placeholder for Settings functionality
        }

        ImGui::EndMenu();
    }
}