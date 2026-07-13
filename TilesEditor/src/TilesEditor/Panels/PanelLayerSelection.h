#pragma once

#include "Panel.h"
#include "Tiles.h"
#include "imgui.h"
#include <vector>
#include <string>
#include <memory>

namespace Tiles
{
    class Command;
    class LayerStack;
    class TileLayer;
}

namespace Tiles::Editor
{
    // Panel for managing the project's layer stack: the layer list with
    // per-layer visibility, selection of the working layer, add/delete/clear and
    // reorder operations, and editing of the selected layer's name/render group.
    // Structural changes go through the Session command system so they are undoable.
    class PanelLayerSelection : public Panel
    {
    public:
        PanelLayerSelection(EditorHost& host) : Panel(host) {}
        ~PanelLayerSelection() = default;

        void Render() override;
        void Update() override;

    private:
        // Main rendering blocks
        void RenderBlockProjectInfo();
        void RenderBlockLayerList();
        void RenderBlockLayerControls();
        void RenderBlockSelectedLayerInfo();

        // Component rendering helpers
        void RenderComponentProjectNameInput(const char* id, const std::string& projectName);
        void RenderComponentLayerItem(const char* id, size_t layerIndex, LayerStack& layerStack);
        void RenderComponentLayerMovementControls(const char* id);
        void RenderComponentLayerOperationControls(const char* id);
        void RenderComponentLayerNameInput(const char* id, const std::string& layerName);
        void RenderComponentLayerProperties(const char* id, const TileLayer& layer);
        void RenderComponentRenderGroupSelection(const char* id, TileLayer& layer);

        // Utility methods
        void ExecuteLayerCommand(std::unique_ptr<Command> command);
        bool HasWorkingLayer() const;
        bool HasLayers() const;
    };
}