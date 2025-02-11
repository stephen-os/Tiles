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

    if(m_ShowNewPopup)
	{
		RenderNewPopup();
	}
}

void HeaderPanel::RenderFile()
{
    // File Menu
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New"))
        {
            m_ShowNewPopup = true;
        }
        if (ImGui::MenuItem("Save"))
        {
            // Placeholder for Save functionality
        }
        if (ImGui::MenuItem("Load"))
        {
            // Placeholder for Load functionality
        }

        if (ImGui::MenuItem("Export"))
        {

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

void HeaderPanel::RenderNewPopup()
{
    ImGui::OpenPopup("New Project");

    if (ImGui::BeginPopupModal("New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter width and height:");

        ImGui::InputInt("Width", &m_NewWidth);
        ImGui::InputInt("Height", &m_NewHeight);

        if (ImGui::Button("Create"))
        {
            m_TileLayers->Create(m_NewWidth, m_NewHeight);
            m_ShowNewPopup = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            m_ShowNewPopup = false;
        }

        ImGui::End();
    }
}