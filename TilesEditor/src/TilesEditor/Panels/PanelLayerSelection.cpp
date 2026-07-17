#include "PanelLayerSelection.h"

#include "../UI/Widgets.h"
#include "../UI/Theme.h"

#include "Core/Logger.h"

#include "Commands/LayerAddCommand.h"
#include "Commands/LayerDeleteCommand.h"
#include "Commands/LayerClearCommand.h"
#include "Commands/LayerMoveUpCommand.h"
#include "Commands/LayerMoveDownCommand.h"

#include "imgui.h"

#include <algorithm>

namespace Tiles::Editor
{
    void PanelLayerSelection::Render()
    {
        ImGui::Begin("Layer Selection", OpenFlag());

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
    }

    void PanelLayerSelection::RenderBlockProjectInfo()
    {
        auto project = Ctx().GetProject();

        UI::SectionHeader("Project");
        RenderComponentProjectNameInput("ProjectName", project->GetProjectName());
    }

    void PanelLayerSelection::RenderComponentProjectNameInput(const char* id, const std::string& projectName)
    {
        char buffer[128];
        size_t copySize = std::min(projectName.size(), sizeof(buffer) - 1);
        std::copy(projectName.begin(), projectName.begin() + copySize, buffer);
        buffer[copySize] = '\0';

        std::string inputId = std::string("##") + id + "_Input";
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputText(inputId.c_str(), buffer, sizeof(buffer)))
            Ctx().GetProject()->SetProjectName(std::string(buffer));
    }

    void PanelLayerSelection::RenderBlockLayerList()
    {
        LayerStack& layerStack = Ctx().GetProject()->GetLayerStack();

        UI::SectionHeader("Layers");

        ImGui::BeginChild("##LayerListChild", ImVec2(0.0f, 260.0f), ImGuiChildFlags_Borders);

        for (size_t i = 0; i < layerStack.GetLayerCount(); i++)
        {
            std::string itemId = "LayerItem_" + std::to_string(i);
            RenderComponentLayerItem(itemId.c_str(), i, layerStack);
        }

        ImGui::EndChild();
    }

    void PanelLayerSelection::RenderComponentLayerItem(const char* id, size_t layerIndex, LayerStack& layerStack)
    {
        TileLayer& layer = layerStack.GetLayer(layerIndex);
        bool isSelected = (layerIndex == Ctx().GetWorkingLayer());

        ImGui::PushID(id);

        // The theme paints the checkmark and the selection highlight in the accent
        // color, so the visibility toggle + selectable need no manual styling.
        bool isVisible = layer.GetVisibility();
        if (ImGui::Checkbox("##Visible", &isVisible))
        {
            layer.SetVisibility(isVisible);
            Ctx().GetProject()->MarkAsModified();
        }
        ImGui::SameLine();

        if (ImGui::Selectable(layer.GetName().c_str(), isSelected))
            Ctx().SetWorkingLayer(layerIndex);

        ImGui::PopID();
    }

    void PanelLayerSelection::RenderBlockLayerControls()
    {
        RenderComponentLayerMovementControls("LayerMovement");
        RenderComponentLayerOperationControls("LayerOperations");
    }

    void PanelLayerSelection::RenderComponentLayerMovementControls(const char* id)
    {
        ImGui::BeginDisabled(!HasWorkingLayer());

        if (ImGui::BeginTable("##MovementButtons", 2, ImGuiTableFlags_SizingStretchSame))
        {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            if (UI::Button("Move Up", UI::ButtonVariant::Default, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
                ExecuteLayerCommand(std::make_unique<LayerMoveUpCommand>(Ctx().GetWorkingLayer()));

            ImGui::TableNextColumn();
            if (UI::Button("Move Down", UI::ButtonVariant::Default, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
                ExecuteLayerCommand(std::make_unique<LayerMoveDownCommand>(Ctx().GetWorkingLayer()));

            ImGui::EndTable();
        }

        ImGui::EndDisabled();
    }

    void PanelLayerSelection::RenderComponentLayerOperationControls(const char* id)
    {
        const bool hasWorkingLayer = HasWorkingLayer();
        const bool hasLayers = HasLayers();

        // Deleting the final layer would leave the project with none, so Delete
        // stays disabled until at least two exist.
        const size_t layerCount = Ctx().GetProject()->GetLayerStack().GetLayerCount();
        const bool canDeleteLayer = hasWorkingLayer && layerCount > 1;

        if (ImGui::BeginTable("##OperationButtons", 3, ImGuiTableFlags_SizingStretchSame))
        {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            if (UI::Button("Add Layer", UI::ButtonVariant::Default, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
                ExecuteLayerCommand(std::make_unique<LayerAddCommand>());

            ImGui::TableNextColumn();
            ImGui::BeginDisabled(!canDeleteLayer);
            if (UI::Button("Delete Layer", UI::ButtonVariant::Danger, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
                ExecuteLayerCommand(std::make_unique<LayerDeleteCommand>(Ctx().GetWorkingLayer()));
            ImGui::EndDisabled();

            ImGui::TableNextColumn();
            ImGui::BeginDisabled(!hasLayers || !hasWorkingLayer);
            if (UI::Button("Clear Layer", UI::ButtonVariant::Danger, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
                ExecuteLayerCommand(std::make_unique<LayerClearCommand>(Ctx().GetWorkingLayer()));
            ImGui::EndDisabled();

            ImGui::EndTable();
        }
    }

    void PanelLayerSelection::RenderBlockSelectedLayerInfo()
    {
        if (!HasWorkingLayer())
            return;

        LayerStack& layerStack = Ctx().GetProject()->GetLayerStack();
        TileLayer& layer = layerStack.GetLayer(Ctx().GetWorkingLayer());

        ImGui::Separator();
        UI::SectionHeader("Layer Properties");

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

        if (UI::BeginPropertyTable("##LayerNameInput"))
        {
            UI::PropertyLabel("Name");

            std::string inputId = std::string("##") + id + "_Input";
            if (ImGui::InputText(inputId.c_str(), buffer, sizeof(buffer)))
            {
                TileLayer& layer = Ctx().GetProject()->GetLayerStack().GetLayer(Ctx().GetWorkingLayer());
                layer.SetName(std::string(buffer));
                Ctx().GetProject()->MarkAsModified();
            }

            UI::EndPropertyTable();
        }
    }

    void PanelLayerSelection::RenderComponentRenderGroupSelection(const char* id, TileLayer& layer)
    {
        if (UI::BeginPropertyTable("##RenderGroupSelection"))
        {
            UI::PropertyLabel("Render Group");

            auto renderGroups = TileLayerUtils::GetAllRenderGroups();
            auto renderGroupNames = TileLayerUtils::GetAllRenderGroupNames();
            auto renderGroupValues = TileLayerUtils::GetAllRenderGroupValues();
            size_t groupCount = TileLayerUtils::GetRenderGroupCount();

            int currentRenderGroup = static_cast<int>(layer.GetRenderGroup());
            int currentIndex = 0;
            for (size_t i = 0; i < groupCount; ++i)
            {
                if (renderGroupValues[i] == currentRenderGroup)
                {
                    currentIndex = static_cast<int>(i);
                    break;
                }
            }

            std::string comboId = std::string("##") + id + "_Combo";
            std::vector<const char*> nameArray(renderGroupNames.begin(), renderGroupNames.end());

            if (ImGui::Combo(comboId.c_str(), &currentIndex, nameArray.data(), static_cast<int>(groupCount)))
            {
                RenderGroup newGroup = renderGroups[currentIndex];
                if (layer.GetRenderGroup() != newGroup)
                {
                    layer.SetRenderGroup(newGroup);
                    Ctx().GetProject()->MarkAsModified();
                }
            }

            UI::EndPropertyTable();
        }
    }

    void PanelLayerSelection::RenderComponentLayerProperties(const char* id, const TileLayer& layer)
    {
        const UI::Theme& theme = UI::GetTheme();

        if (UI::BeginPropertyTable("##LayerProperties"))
        {
            UI::PropertyLabel("Tile Count");
            ImGui::TextColored(theme.Accent, "%zu", layer.GetTileCount());

            UI::PropertyLabel("Render Group");
            ImGui::TextColored(theme.Accent, "%s", TileLayerUtils::GetRenderGroupName(layer.GetRenderGroup()));

            UI::PropertyLabel("Visible");
            if (layer.GetVisibility())
                ImGui::TextColored(theme.Success, "Yes");
            else
                ImGui::TextColored(theme.Danger, "No");

            UI::EndPropertyTable();
        }
    }

    void PanelLayerSelection::ExecuteLayerCommand(std::unique_ptr<Command> command)
    {
        if (command)
            Ctx().ExecuteCommand(std::move(command));
    }

    bool PanelLayerSelection::HasWorkingLayer() const
    {
        return Ctx().HasWorkingLayer();
    }

    bool PanelLayerSelection::HasLayers() const
    {
        return !Ctx().GetProject()->GetLayerStack().IsEmpty();
    }
}
