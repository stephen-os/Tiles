#include "HeaderPanel.h"

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "../Core/Project.h"

void HeaderPanel::Render()
{
    if (ImGui::BeginMainMenuBar())
    {
        RenderFile();
        RenderEdit();
        RenderOptions();
    }

    ImGui::EndMainMenuBar();

    if (m_ShowNewPopup)
    {
        RenderNewPopup();
    }
}

void HeaderPanel::RenderFile()
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New"))
        {
            m_ShowNewPopup = true;
        }
        if (ImGui::MenuItem("Save"))
        {
            IGFD::FileDialogConfig config;
            config.path = ".";
            config.flags = ImGuiFileDialogFlags_Modal;
            config.countSelectionMax = 1;

            ImGuiFileDialog::Instance()->OpenDialog("SaveProject", "Save Project", ".json", config);
        }
        if (ImGui::MenuItem("Load"))
        {
            IGFD::FileDialogConfig config;
            config.path = ".";
            config.flags = ImGuiFileDialogFlags_Modal;
            config.countSelectionMax = 1;

            ImGuiFileDialog::Instance()->OpenDialog("LoadProject", "Load Project", ".json", config);
        }
        if (ImGui::MenuItem("Export"))
        {
            IGFD::FileDialogConfig config;
            config.path = ".";
            config.flags = ImGuiFileDialogFlags_Modal;
            config.countSelectionMax = 1;

            ImGuiFileDialog::Instance()->OpenDialog("ExportProject", "Export Tile Map", ".png", config);
        }
        ImGui::EndMenu();
    }

    // Handle Save Dialog
    if (ImGuiFileDialog::Instance()->Display("SaveProject", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            Project::Save(filePath, m_Layers, m_Atlas);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // Handle Load Dialog
    if (ImGuiFileDialog::Instance()->Display("LoadProject", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            Project::Load(filePath, m_Layers, m_Atlas);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // Handle Export Dialog
    if (ImGuiFileDialog::Instance()->Display("ExportProject", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            TileExporter exporter; 
            std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            // Fix
            // exporter.Export(m_Layers, m_Atlas, filePath);
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void HeaderPanel::RenderEdit()
{
    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Undo"))
        {
        }
        if (ImGui::MenuItem("Redo"))
        {
        }
        ImGui::EndMenu();
    }
}

void HeaderPanel::RenderOptions()
{
    if (ImGui::BeginMenu("Options"))
    {
        if (ImGui::MenuItem("Settings"))
        {
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
            m_Layers->Clear(); 
            m_Layers->Resize(m_NewWidth, m_NewHeight);
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
