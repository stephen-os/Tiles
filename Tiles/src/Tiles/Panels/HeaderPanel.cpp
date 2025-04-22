#include "HeaderPanel.h"

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "../Core/Project.h"
#include "../Core/Modes.h"

#include "Lumina/Core/Log.h"

#include "GLFW/glfw3.h"

namespace Tiles
{
    void HeaderPanel::Render()
    {
        if (ImGui::BeginMainMenuBar())
        {
            RenderFile();
            RenderEdit();
            RenderExample();
            RenderHelp(); 
        }

        ImGui::EndMainMenuBar();

        if (m_ShowNewPopup)
        {
            Modes::SetCurrentMode(Modes::Mode::None);
            RenderNewPopup();
        }

        if (m_ShowRenderMatrixPopup)
        {
            Modes::SetCurrentMode(Modes::Mode::None);
            RenderRenderMatrixPopup();
        }

        if (m_ShowAboutPopup)
		{
			RenderAboutPopup();
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
            }
            if (ImGui::MenuItem("Exit"))
            {
                glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
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
                // Undo
                if (m_CommandHistory->CanUndo())
				{
					m_CommandHistory->Undo();
				}
            }
            if (ImGui::MenuItem("Redo"))
            {
                // Redo
                if (m_CommandHistory->CanRedo())
                {
                    m_CommandHistory->Redo();
                }
            }
            ImGui::EndMenu();
        }
    }

    void HeaderPanel::RenderExample()
    {
        if (ImGui::BeginMenu("Examples"))
		{
            if (ImGui::MenuItem("Example Project 1"))
            {
                Project::Load("res/maps/example_map1.json", m_Layers, m_Atlas);
            }
            if (ImGui::MenuItem("Example Project 2"))
            {
                Project::Load("res/maps/example_map2.json", m_Layers, m_Atlas);
            }
            ImGui::EndMenu();
		}
    }

    void HeaderPanel::RenderHelp()
    {
        if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
			{
                m_ShowAboutPopup = true; 
			}
			ImGui::EndMenu();
		}
    }

    void HeaderPanel::RenderNewPopup()
    {
        ImGui::OpenPopup("New Project");

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Enter width and height:");
            ImGui::InputInt("Width", &m_NewWidth);
            ImGui::InputInt("Height", &m_NewHeight);

            float windowWidth = ImGui::GetWindowSize().x;
            float createButtonWidth = ImGui::CalcTextSize("Create").x + ImGui::GetStyle().FramePadding.x * 2;
            float cancelButtonWidth = ImGui::CalcTextSize("Cancel").x + ImGui::GetStyle().FramePadding.x * 2;
            ImGui::SetCursorPosX((windowWidth - createButtonWidth - cancelButtonWidth - 2) * 0.5f);

            if (ImGui::Button("Create"))
            {
                m_Layers->ClearAllLayers();
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

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			ImGui::BeginPopupModal("Render Matrix", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("No layers available");
            
            float windowWidth = ImGui::GetWindowSize().x;
            float buttonWidth = ImGui::CalcTextSize("Close").x + ImGui::GetStyle().FramePadding.x * 2;
            ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

            if (ImGui::Button("Close"))
            {
                m_ShowRenderMatrixPopup = false;
            }

			ImGui::EndPopup();

            return; 
		}

        ImGui::OpenPopup("Render Matrix");

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

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

            if (ImGui::BeginTable("CheckboxTable", static_cast<int>(cols + 1), ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
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

					Layer& layer = m_Layers->GetLayer(row);

                    for (int col = 0; col < cols; col++)
                    {
                        ImGui::TableSetColumnIndex(col + 1);
                        std::string label = "##chk" + std::to_string(row) + "_" + std::to_string(col);

                        Layer& layer = m_Layers->GetLayer(row);
						bool checked = (layer.GetRenderGroup() == col);

                        if (ImGui::Checkbox(label.c_str(), &checked))
                        {
							if (checked)
							{
								layer.SetRenderGroup(col);
							}
							else
							{
								layer.SetRenderGroup(-1);
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
				m_ExportAttributes.Filepath = m_ExportFilePath;
				m_ExportAttributes.Filename = m_ExportFileName;
				TileRenderer::ExportLayers(*m_Layers, *m_Atlas, m_ExportAttributes);
            }

            ImGui::SameLine();

            float windowWidth = ImGui::GetWindowSize().x;
            float buttonWidth = ImGui::CalcTextSize("Cancel").x + ImGui::GetStyle().FramePadding.x * 2;
            ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

            if (ImGui::Button("Cancel"))
            {
                m_ShowRenderMatrixPopup = false;
            }

            ImGui::EndPopup();
        }
    }

    void HeaderPanel::RenderAboutPopup()
    {
        ImGui::OpenPopup("About");

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Suggested Workflow:");
            ImGui::Text("* Go to File->New Project to create a new project with specific dimensions.");
            ImGui::Text("* Browse a texture atlas, load it and resize it.");
            ImGui::Text("* Add layers via the Add Layer button.");
            ImGui::Text("* Select a layer and paint, erase, or fill it.");
            ImGui::Text("* Export the project as a PNG.");
            ImGui::Text("* Optional: Save project for future editing.");
            
            ImGui::Separator();
            ImGui::Text("Example Projects:");
            ImGui::Text("There are 2 example projects that can be viewed under Examples->Example Project 1 and Examples->Example Project 2.");

            ImGui::Separator();
            ImGui::Text("Features:");
            ImGui::Text("* Painting, Erasing, and Filling tiles per layer.");
            ImGui::Text("* Adding, Removing, Clearing, and Renaming of a selected layer.");
            ImGui::Text("* Resizing texture atlas on the fly.");
            ImGui::Text("* Saving and Loading of a project as JSON.");
            ImGui::Text("* Exporting of a project as a PNG.");
            ImGui::Text("* * Rendering Matrix for grouping layers for export.");
            ImGui::Text("* * Export Multiple PNGs based on groupings.");
            ImGui::Text("* Shortcuts for Undo/Redo.");

            ImGui::Separator();
            ImGui::Text("Technical Dependencies:");
            ImGui::Text("Lumina - Custom Render/Application Starter - https://github.com/Resetss/Lumina");
            ImGui::Text("* ImGui - https://github.com/Resetss/imgui");
			ImGui::Text("* ImGuiFileDialog - https://github.com/Resetss/ImGuiFileDialog");
		    ImGui::Text("* GLFW - https://github.com/Resetss/glfw");
            ImGui::Text("* Glad - https://github.com/Resetss/glad");
            ImGui::Text("* GLM - https://github.com/g-truc/glm"); 

            ImGui::Separator();
            ImGui::Text("Shortcuts:");
            ImGui::Text("CTRL + Z - Undo");
            ImGui::Text("CTRL + Y - Redo");
            ImGui::Text("MIDDLE MOUSE - Drag");
            ImGui::Text("SCROLL WHEEL - Scroll");

            ImGui::Separator();

            float windowWidth = ImGui::GetWindowSize().x;
            float buttonWidth = ImGui::CalcTextSize("Close").x + ImGui::GetStyle().FramePadding.x * 2;
            ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

            if (ImGui::Button("Close"))
            {
                m_ShowAboutPopup = false;
            }
            ImGui::End();
        }
    }
}