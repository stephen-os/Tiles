#include "HeaderPanel.h"

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "../Core/Project.h"

#include <iostream>

namespace Tiles
{

    void HeaderPanel::OnUIRender()
    {
        if (ImGui::BeginMainMenuBar())
        {
            // Clearing texture selection to prevent drawing to viewport when in header menues. 
            // We may want to manage drawing states a differnt way instead of clearing selection. 
            m_TextureSelection->Clear();

            RenderFile();
            RenderEdit();
            RenderOptions();

        }

        ImGui::EndMainMenuBar();

        if (m_ShowNewPopup)
        {
            m_TextureSelection->Clear();

            RenderNewPopup();
        }

        if (m_ShowRenderMatrixPopup)
        {
            m_TextureSelection->Clear();

            RenderRenderMatrixPopup();
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
                m_ShowRenderMatrixPopup = true;

                size_t rows = m_Layers->GetSize();
                size_t cols = m_Layers->GetSize();

                m_Checkboxes.clear();
                m_Checkboxes.resize(rows * cols, false);

                for (size_t row = 0; row < rows; row++)
				{
				    m_Checkboxes[row * cols] = true;
				}
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
    }

    void HeaderPanel::RenderEdit()
    {
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo"))
            {
                m_State->Undo();
            }
            if (ImGui::MenuItem("Redo"))
            {
                m_State->Redo();
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

    void HeaderPanel::RenderRenderMatrixPopup()
    {
        if (m_Layers->IsEmpty())
		{
			ImGui::OpenPopup("Render Matrix");
			ImGui::BeginPopupModal("Render Matrix", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("No layers available");
            
            if (ImGui::Button("Cancel"))
            {
                m_ShowRenderMatrixPopup = false;
            }

			ImGui::EndPopup();

            return; 
		}

        ImGui::OpenPopup("Render Matrix");

        if (ImGui::BeginPopupModal("Render Matrix", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Define Layer Groupings:");
            ImGui::Separator();

            ImVec2 windowSize = ImGui::GetWindowSize();
            float contentWidth = windowSize.x * 0.8f;
            ImGui::SetNextItemWidth(contentWidth);
            ImGui::BeginGroup();

            size_t rows = m_Layers->GetSize();
            size_t cols = m_Layers->GetSize();

            if (ImGui::BeginTable("CheckboxTable", cols + 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
            {
                ImGui::TableSetupColumn("Layer", ImGuiTableColumnFlags_WidthFixed);

                for (int col = 0; col < cols; col++)
                {
                    std::string groupingLabel = std::to_string(col + 1);
                    ImGui::TableSetupColumn(groupingLabel.c_str(), ImGuiTableColumnFlags_WidthFixed);
                }

                ImGui::TableHeadersRow();

                for (int row = 0; row < rows; row++)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Layer %d", row + 1);

                    for (int col = 0; col < cols; col++)
                    {
                        ImGui::TableSetColumnIndex(col + 1);
                        std::string label = "##chk" + std::to_string(row) + "_" + std::to_string(col);

                        bool checked = m_Checkboxes[row * cols + col];
                        if (ImGui::Checkbox(label.c_str(), &checked))
                        {
                            m_Checkboxes[row * cols + col] = checked;

                            for (size_t otherCol = 0; otherCol < cols; otherCol++)
                            {
                                if (otherCol != col)
                                    m_Checkboxes[row * cols + otherCol] = false;
                            }
                        }
                    }
                }

                ImGui::EndTable();
            }

            ImGui::EndGroup();

            ImGui::Separator();

            ImGui::Text("Export Settings:");

            ImGui::InputText("Export Filepath", m_ExportFilePath, IM_ARRAYSIZE(m_ExportFilePath));

            ImGui::SameLine();

            if (ImGui::Button("Browse"))
            {
                IGFD::FileDialogConfig config;
                config.path = ".";
                config.flags = ImGuiFileDialogFlags_Modal;
                config.countSelectionMax = 1;

                ImGuiFileDialog::Instance()->OpenDialog("ExportDir", "Select Export Directory", nullptr, config);
            }

            // Process file dialog selection
            if (ImGuiFileDialog::Instance()->Display("ExportDir", ImGuiWindowFlags_NoCollapse, ImVec2(800, 600)))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    std::string path = ImGuiFileDialog::Instance()->GetCurrentPath();
                    strncpy_s(m_ExportFilePath, path.c_str(), sizeof(m_ExportFilePath) - 1);
                }
                ImGuiFileDialog::Instance()->Close();
            }

            ImGui::InputText("Export Filename", m_ExportFileName, IM_ARRAYSIZE(m_ExportFileName));

			ImGui::Separator();

            if (ImGui::Button("Export"))
            {
                std::vector<size_t> groupings;
                groupings.clear();
                groupings.resize(m_Layers->GetSize(), 0);

                for (int row = 0; row < rows; row++)
                {
                    for (int col = 0; col < cols; col++)
                    {
                        if (m_Checkboxes[row * cols + col])
                            groupings[col]++;
                    }
                }

                Exporter exporter;
                std::string path(m_ExportFilePath);
                std::string name(m_ExportFileName);
                exporter.Export(m_Layers, m_Atlas, path, name, groupings);
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
            {
                m_ShowRenderMatrixPopup = false;
            }

            ImGui::EndPopup();
        }
    }


}