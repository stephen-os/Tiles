#include "PanelLayerSelection.h"

#include "Core/Constants.h"

#include "Lumina/Core/Log.h"

#include "../Core/Commands/LayerAddCommand.h"
#include "../Core/Commands/LayerDeleteCommand.h"
#include "../Core/Commands/LayerClearCommand.h"
#include "../Core/Commands/LayerMoveUpCommand.h"
#include "../Core/Commands/LayerMoveDownCommand.h"

#include "imgui.h"

namespace Tiles
{
    void PanelLayerSelection::Render()
    {
        ImGui::Begin("Layer Selection");

        if (!m_Context)
        {
            ImGui::TextColored(UI::Color::TextHint, "No active project->");
            ImGui::End();
            return;
        }

        ImGui::PushID("LayerSelection");

        RenderBlockProjectInfo();
        ImGui::Separator();
        RenderBlockLayerList();
        RenderBlockLayerControls();
        RenderBlockSelectedLayerInfo();

        ImGui::PopID();
        ImGui::End();
    }

    void PanelLayerSelection::Update()
    {
        // No specific update logic required for this panel
    }

    void PanelLayerSelection::RenderBlockProjectInfo()
    {
        auto project = m_Context->GetProject();

        // Section title
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);
        ImGui::Button("project-> Name", ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight));
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();

        RenderComponentProjectNameInput("ProjectName", project->GetProjectName());
    }

    void PanelLayerSelection::RenderComponentProjectNameInput(const char* id, const std::string& projectName)
    {
        char buffer[128];

        size_t copySize = std::min(projectName.size(), sizeof(buffer) - 1);
        std::copy(projectName.begin(), projectName.begin() + copySize, buffer);
        buffer[copySize] = '\0';

        std::string inputId = std::string("##") + id + "_Input";
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, UI::Layer::ItemBackground);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, UI::Layer::ItemHover);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, UI::Layer::ItemHover);
        ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        if (ImGui::InputText(inputId.c_str(), buffer, sizeof(buffer)))
        {
            m_Context->GetProject()->SetProjectName(std::string(buffer));
        }

        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
    }

    void PanelLayerSelection::RenderBlockLayerList()
    {
        LayerStack& layerStack = m_Context->GetProject()->GetLayerStack();

        // Section title
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);
        ImGui::Button("Layers", ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight));
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();

        // Layer list container - no rounding
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, UI::Layer::ItemBackground);

        ImVec2 listSize(0, UI::List::LayerListHeight);
        ImGui::BeginChild("##LayerListChild", listSize, true, ImGuiWindowFlags_None);

        // Render each layer
        for (size_t i = 0; i < layerStack.GetLayerCount(); i++)
        {
            std::string itemId = "LayerItem_" + std::to_string(i);
            RenderComponentLayerItem(itemId.c_str(), i, layerStack);
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    void PanelLayerSelection::RenderComponentLayerItem(const char* id, size_t layerIndex, LayerStack& layerStack)
    {
        TileLayer& layer = layerStack.GetLayer(layerIndex);
        bool isSelected = (layerIndex == m_Context->GetWorkingLayer());

        ImGui::PushID(id);

        // Visibility checkbox with orange styling and border
        bool isVisible = layer.GetVisibility();
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_CheckMark, UI::Layer::OrangeAccent);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, UI::Layer::ItemBackground);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, UI::Layer::ItemHover);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, UI::Layer::ItemHover);
        ImGui::PushStyleColor(ImGuiCol_Border, UI::Layer::OrangeAccent);

        if (ImGui::Checkbox("##Visible", &isVisible))
        {
            layer.SetVisibility(isVisible);
            m_Context->GetProject()->MarkAsModified();
        }

        ImGui::PopStyleColor(5);
        ImGui::PopStyleVar(2);
        ImGui::SameLine();

        // Layer selection with orange selection color
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

        if (isSelected)
        {
            ImGui::PushStyleColor(ImGuiCol_Header, UI::Layer::OrangeAccent);
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, UI::Layer::OrangeAccent);
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, UI::Layer::OrangeAccent);
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Header, UI::Layer::ItemBackground);
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, UI::Layer::ItemHover);
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, UI::Layer::ItemHover);
        }

        ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);

        if (ImGui::Selectable(layer.GetName().c_str(), isSelected))
        {
            m_Context->SetWorkingLayer(layerIndex);
        }

        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
        ImGui::PopID();
    }

    void PanelLayerSelection::RenderBlockLayerControls()
    {
        RenderComponentLayerMovementControls("LayerMovement");
        ImGui::Separator();
        RenderComponentLayerOperationControls("LayerOperations");
    }

    void PanelLayerSelection::RenderComponentLayerMovementControls(const char* id)
    {
        bool hasWorkingLayer = HasWorkingLayer();

        // Style for buttons
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(UI::Component::SpaceBetween / 2.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, UI::Layer::ItemBackground);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Layer::ItemHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Layer::OrangeAccent);
        ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);

        if (!hasWorkingLayer)
        {
            ImGui::BeginDisabled();
        }

        // Movement buttons in table
        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;
        if (ImGui::BeginTable("##MovementButtons", 2, tableFlags))
        {
            ImGui::TableSetupColumn("MoveUp", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("MoveDown", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            if (ImGui::Button("Move Up", ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight)))
            {
                auto command = std::make_unique<LayerMoveUpCommand>(m_Context->GetWorkingLayer());
                ExecuteLayerCommand(std::move(command));
            }

            ImGui::TableNextColumn();
            if (ImGui::Button("Move Down", ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight)))
            {
                auto command = std::make_unique<LayerMoveDownCommand>(m_Context->GetWorkingLayer());
                ExecuteLayerCommand(std::move(command));
            }

            ImGui::EndTable();
        }

        if (!hasWorkingLayer)
        {
            ImGui::EndDisabled();
        }

        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(2);
    }

    void PanelLayerSelection::RenderComponentLayerOperationControls(const char* id)
    {
        bool hasWorkingLayer = HasWorkingLayer();
        bool hasLayers = HasLayers();

        // Style for buttons
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(UI::Component::SpaceBetween / 2.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, UI::Layer::ItemBackground);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Layer::ItemHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Layer::OrangeAccent);
        ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);

        // Operation buttons in table
        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;
        if (ImGui::BeginTable("##OperationButtons", 3, tableFlags))
        {
            ImGui::TableSetupColumn("Add", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Clear", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            // Add Layer button (always enabled)
            ImGui::TableNextColumn();
            if (ImGui::Button("Add Layer", ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight)))
            {
                auto command = std::make_unique<LayerAddCommand>();
                ExecuteLayerCommand(std::move(command));
            }

            // Delete and Clear buttons (conditionally enabled)
            if (!hasLayers || !hasWorkingLayer)
            {
                ImGui::BeginDisabled();
            }

            ImGui::TableNextColumn();
            if (ImGui::Button("Delete Layer", ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight)))
            {
                auto command = std::make_unique<LayerDeleteCommand>(m_Context->GetWorkingLayer());
                ExecuteLayerCommand(std::move(command));
            }

            ImGui::TableNextColumn();
            if (ImGui::Button("Clear Layer", ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight)))
            {
                auto command = std::make_unique<LayerClearCommand>(m_Context->GetWorkingLayer());
                ExecuteLayerCommand(std::move(command));
            }

            if (!hasLayers || !hasWorkingLayer)
            {
                ImGui::EndDisabled();
            }

            ImGui::EndTable();
        }

        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(2);
    }

    void PanelLayerSelection::RenderBlockSelectedLayerInfo()
    {
        if (!HasWorkingLayer())
        {
            return;
        }

        LayerStack& layerStack = m_Context->GetProject()->GetLayerStack();
        TileLayer& layer = layerStack.GetLayer(m_Context->GetWorkingLayer());

        ImGui::Separator();

        // Section title
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);
        ImGui::Button("Layer Properties", ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight));
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();

        RenderComponentLayerNameInput("LayerName", layer.GetName());
        RenderComponentRenderGroupSelection("RenderGroup", layer);
        RenderComponentLayerProperties("LayerProperties", layer);
    }

    void PanelLayerSelection::RenderComponentLayerNameInput(const char* id, const std::string& layerName)
    {
        char buffer[64];
        size_t copySize = std::min(layerName.size(), sizeof(buffer) - 1);
        std::copy(layerName.begin(), layerName.begin() + copySize, buffer);
        buffer[copySize] = '\0';

        // Use table for consistent alignment and spacing
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(UI::Component::SpaceBetween * 0.5f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

        if (ImGui::BeginTable("##LayerNameInput", 2, tableFlags))
        {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            // Label column
            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Button, UI::Color::BackgroundMedium);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Color::BackgroundMedium);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Color::BackgroundMedium);
            ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);
            ImGui::Button("Layer Name", ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight));
            ImGui::PopStyleColor(4);

            // Input column
            ImGui::TableNextColumn();
            std::string inputId = std::string("##") + id + "_Input";
            float framePaddingY = (UI::Component::ButtonHeight - ImGui::GetTextLineHeight()) * 0.5f;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(UI::Component::FramePadding, framePaddingY));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, UI::Layer::ItemBackground);
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, UI::Layer::ItemHover);
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, UI::Layer::ItemHover);
            ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

            if (ImGui::InputText(inputId.c_str(), buffer, sizeof(buffer)))
            {
                LayerStack& layerStack = m_Context->GetProject()->GetLayerStack();
                TileLayer& layer = layerStack.GetLayer(m_Context->GetWorkingLayer());
                layer.SetName(std::string(buffer));
                m_Context->GetProject()->MarkAsModified();
            }

            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar();

            ImGui::EndTable();
        }

        ImGui::PopStyleVar(2);
    }

    void PanelLayerSelection::RenderComponentLayerProperties(const char* id, const TileLayer& layer)
    {
        // Enhanced styling with background colors and borders - increased padding
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(UI::Component::SpaceBetween * 2.5f, UI::Component::SpaceBetween * 1.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);
        ImGui::PushStyleColor(ImGuiCol_TableRowBg, UI::Layer::ItemBackground);
        ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImVec4(0.18f, 0.18f, 0.18f, 1.0f)); // Slightly lighter alternate rows

        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit |
            ImGuiTableFlags_NoHostExtendX |
            ImGuiTableFlags_RowBg |           // Enable row backgrounds
            ImGuiTableFlags_BordersInnerV;    // Add vertical borders

        if (ImGui::BeginTable("##LayerProperties", 2, tableFlags))
        {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

            // Dimensions
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), " Dimensions:");
            ImGui::TableNextColumn();
            ImGui::TextColored(UI::Layer::OrangeAccent, "%dx%d", layer.GetWidth(), layer.GetHeight());

            // Tile Count
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), " Tile Count:");
            ImGui::TableNextColumn();
            ImGui::TextColored(UI::Layer::OrangeAccent, "%zu", layer.GetTileCount());

            // Render Group
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), " Render Group:");
            ImGui::TableNextColumn();

            const char* groupName = TileLayerUtils::GetRenderGroupName(layer.GetRenderGroup());
            ImGui::TextColored(UI::Layer::OrangeAccent, "%s", groupName);

            // Visible - with colored indicator
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), " Visible:");
            ImGui::TableNextColumn();
            if (layer.GetVisibility()) {
                ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Yes");  // Green for visible
            }
            else {
                ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "No");   // Red for hidden
            }

            ImGui::EndTable();
        }

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);
    }

    void PanelLayerSelection::RenderComponentRenderGroupSelection(const char* id, TileLayer& layer)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(UI::Component::SpaceBetween * 0.5f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

        if (ImGui::BeginTable("##RenderGroupSelection", 2, tableFlags))
        {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Selection", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            // Label column
            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Button, UI::Color::BackgroundMedium);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Color::BackgroundMedium);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Color::BackgroundMedium);
            ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);
            ImGui::Button("Render Group", ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight));
            ImGui::PopStyleColor(4);

            // Selection column
            ImGui::TableNextColumn();
            float framePaddingY = (UI::Component::ButtonHeight - ImGui::GetTextLineHeight()) * 0.5f;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(UI::Component::FramePadding, framePaddingY));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, UI::Layer::ItemBackground);
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, UI::Layer::ItemHover);
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, UI::Layer::ItemHover);
            ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);

            // Use TileLayerUtils for dynamic render group data
            auto renderGroups = TileLayerUtils::GetAllRenderGroups();
            auto renderGroupNames = TileLayerUtils::GetAllRenderGroupNames();
            auto renderGroupValues = TileLayerUtils::GetAllRenderGroupValues();
            size_t groupCount = TileLayerUtils::GetRenderGroupCount();

            int currentRenderGroup = static_cast<int>(layer.GetRenderGroup());
            int currentIndex = 0;

            // Find current selection index
            for (size_t i = 0; i < groupCount; ++i)
            {
                if (renderGroupValues[i] == currentRenderGroup)
                {
                    currentIndex = static_cast<int>(i);
                    break;
                }
            }

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            std::string comboId = std::string("##") + id + "_Combo";

            std::vector<const char*> nameArray(renderGroupNames.begin(), renderGroupNames.end());

            if (ImGui::Combo(comboId.c_str(), &currentIndex, nameArray.data(), static_cast<int>(groupCount)))
            {
                RenderGroup newGroup = renderGroups[currentIndex];
                if (layer.GetRenderGroup() != newGroup)
                {
                    layer.SetRenderGroup(newGroup);
                    m_Context->GetProject()->MarkAsModified();
                }
            }

            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar();

            ImGui::EndTable();
        }

        ImGui::PopStyleVar(2);
    }

    void PanelLayerSelection::ExecuteLayerCommand(std::unique_ptr<Command> command)
    {
        if (command && m_Context)
        {
            m_Context->ExecuteCommand(std::move(command));
        }
    }

    bool PanelLayerSelection::HasWorkingLayer() const
    {
        return m_Context && m_Context->HasWorkingLayer();
    }

    bool PanelLayerSelection::HasLayers() const
    {
        return m_Context && !m_Context->GetProject()->GetLayerStack().IsEmpty();
    }
}