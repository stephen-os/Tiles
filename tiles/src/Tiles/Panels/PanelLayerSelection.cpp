#include "PanelLayerSelection.h"

#include "Lumina/Core/Log.h"

#include "../Core/Commands/LayerAddCommand.h"
#include "../Core/Commands/LayerDeleteCommand.h"
#include "../Core/Commands/LayerClearCommand.h"
#include "../Core/Commands/LayerMoveUpCommand.h"
#include "../Core/Commands/LayerMoveDownCommand.h"

#include "imgui.h"

namespace Tiles
{
    // UI layout constants
    namespace UILayout
    {
        static constexpr float LAYER_LIST_HEIGHT = 300.0f;
        static constexpr float CHILD_ROUNDING = 5.0f;
        static constexpr size_t PROJECT_NAME_BUFFER_SIZE = 256;
        static constexpr size_t LAYER_NAME_BUFFER_SIZE = 128;
    }

    // UI text constants
    namespace UIText
    {
        static constexpr const char* WINDOW_TITLE = "Layer Selection";
        static constexpr const char* NO_ACTIVE_PROJECT = "No active project";
        static constexpr const char* PROJECT_NAME_LABEL = "Project Name";
        static constexpr const char* LAYERS_LABEL = "Layers";
        static constexpr const char* LAYER_NAME_LABEL = "Layer Name";
        static constexpr const char* MOVE_UP_BUTTON = "Move Up";
        static constexpr const char* MOVE_DOWN_BUTTON = "Move Down";
        static constexpr const char* ADD_LAYER_BUTTON = "Add Layer";
        static constexpr const char* DELETE_LAYER_BUTTON = "Delete Layer";
        static constexpr const char* CLEAR_LAYER_BUTTON = "Clear Layer";
    }

    // Color scheme
    namespace ColorScheme
    {
        static const ImVec4 NO_PROJECT_COLOR = { 0.7f, 0.7f, 0.7f, 1.0f };
    }

    void PanelLayerSelection::Render()
    {
        ImGui::Begin(UIText::WINDOW_TITLE);

        if (!HasContext())
        {
            ImGui::TextColored(ColorScheme::NO_PROJECT_COLOR, UIText::NO_ACTIVE_PROJECT);
            ImGui::End();
            return;
        }

        RenderProjectInfo();
        ImGui::Separator();

        RenderLayerList();
        RenderLayerControls();
        RenderSelectedLayerInfo();

        ImGui::End();
    }

    void PanelLayerSelection::Update()
    {
        // This panel does not require any specific update logic
	}

    void PanelLayerSelection::RenderProjectInfo()
    {
        Project& project = m_Context->GetProject();

        ImGui::Text(UIText::PROJECT_NAME_LABEL);

        // Project name input field
        char buffer[UILayout::PROJECT_NAME_BUFFER_SIZE];
        strncpy_s(buffer, project.GetProjectName().c_str(), sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';

        if (ImGui::InputText("###ProjectName", buffer, sizeof(buffer)))
        {
            project.SetProjectName(std::string(buffer));
        }
    }

    void PanelLayerSelection::RenderLayerList()
    {
        LayerStack& layerStack = m_Context->GetProject().GetLayerStack();

        ImGui::Text(UIText::LAYERS_LABEL);

        // Style the layer list container
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, UILayout::CHILD_ROUNDING);

        ImVec2 listSize(0, UILayout::LAYER_LIST_HEIGHT);
        ImGui::BeginChild("LayerBox", listSize, true, ImGuiWindowFlags_None);

        // Style layer items
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered]);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive]);

        // Render each layer
        for (size_t i = 0; i < layerStack.GetLayerCount(); i++)
        {
            RenderLayerItem(i, layerStack);
        }

        ImGui::PopStyleColor(3);
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }

    void PanelLayerSelection::RenderLayerItem(size_t layerIndex, LayerStack& layerStack)
    {
        ImGui::PushID(static_cast<int>(layerIndex));

        TileLayer& layer = layerStack.GetLayer(layerIndex);

        // Visibility checkbox
        bool isVisible = layer.GetVisibility();
        if (ImGui::Checkbox("##Visible", &isVisible))
        {
            layer.SetVisibility(isVisible);
            m_Context->GetProject().MarkAsModified();
        }

        ImGui::SameLine();

        // Layer selection
        bool isSelected = (layerIndex == m_Context->GetWorkingLayer());
        if (ImGui::Selectable(layer.GetName().c_str(), isSelected))
        {
            m_Context->SetWorkingLayer(layerIndex);
        }

        ImGui::PopID();
    }

    void PanelLayerSelection::RenderLayerControls()
    {
        RenderLayerMovementControls();
        ImGui::Separator();
        RenderLayerOperationControls();
    }

    void PanelLayerSelection::RenderLayerMovementControls()
    {
        bool hasWorkingLayer = m_Context->HasWorkingLayer();

        // Move Up button
        if (!hasWorkingLayer)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button(UIText::MOVE_UP_BUTTON))
        {
            auto command = std::make_unique<LayerMoveUpCommand>(m_Context->GetWorkingLayer());
            ExecuteLayerCommand(std::move(command));
        }

        ImGui::SameLine();

        // Move Down button
        if (ImGui::Button(UIText::MOVE_DOWN_BUTTON))
        {
            auto command = std::make_unique<LayerMoveDownCommand>(m_Context->GetWorkingLayer());
            ExecuteLayerCommand(std::move(command));
        }

        if (!hasWorkingLayer)
        {
            ImGui::EndDisabled();
        }
    }

    void PanelLayerSelection::RenderLayerOperationControls()
    {
        LayerStack& layerStack = m_Context->GetProject().GetLayerStack();
        bool hasWorkingLayer = m_Context->HasWorkingLayer();
        bool hasLayers = !layerStack.IsEmpty();

        // Add Layer button (always enabled)
        if (ImGui::Button(UIText::ADD_LAYER_BUTTON))
        {
            auto command = std::make_unique<LayerAddCommand>();
            ExecuteLayerCommand(std::move(command));
        }

        ImGui::SameLine();

        // Delete Layer button
        if (!hasLayers || !hasWorkingLayer)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button(UIText::DELETE_LAYER_BUTTON))
        {
            auto command = std::make_unique<LayerDeleteCommand>(m_Context->GetWorkingLayer());
            ExecuteLayerCommand(std::move(command));
        }

        ImGui::SameLine();

        // Clear Layer button
        if (ImGui::Button(UIText::CLEAR_LAYER_BUTTON))
        {
            auto command = std::make_unique<LayerClearCommand>(m_Context->GetWorkingLayer());
            ExecuteLayerCommand(std::move(command));
        }

        if (!hasLayers || !hasWorkingLayer)
        {
            ImGui::EndDisabled();
        }
    }

    void PanelLayerSelection::RenderSelectedLayerInfo()
    {
        if (!m_Context->HasWorkingLayer())
        {
            return;
        }

        LayerStack& layerStack = m_Context->GetProject().GetLayerStack();
        ImGui::Separator();

        TileLayer& layer = layerStack.GetLayer(m_Context->GetWorkingLayer());

        // Layer name input field
        char buffer[UILayout::LAYER_NAME_BUFFER_SIZE];
        strncpy_s(buffer, layer.GetName().c_str(), sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';

        if (ImGui::InputText(UIText::LAYER_NAME_LABEL, buffer, sizeof(buffer)))
        {
            layer.SetName(std::string(buffer));
            m_Context->GetProject().MarkAsModified();
        }

        // Additional layer information
        RenderLayerProperties(layer);
    }

    void PanelLayerSelection::RenderLayerProperties(const TileLayer& layer)
    {
        ImGui::Spacing();

        // Layer statistics
        ImGui::Text("Layer Properties:");
        ImGui::Indent();

        ImGui::Text("Dimensions: %dx%d", layer.GetWidth(), layer.GetHeight());
        ImGui::Text("Tile Count: %zu", layer.GetTileCount());
        ImGui::Text("Render Group: %d", layer.GetRenderGroup());
        ImGui::Text("Visible: %s", layer.GetVisibility() ? "Yes" : "No");

        ImGui::Unindent();
    }

    void PanelLayerSelection::ExecuteLayerCommand(std::unique_ptr<Command> command)
    {
        if (command && m_Context)
        {
            m_Context->ExecuteCommand(std::move(command));
        }
    }
}