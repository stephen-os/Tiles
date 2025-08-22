#pragma once

#include "Panel.h"
#include "Lumina/Lumina.h"
#include "imgui.h"
#include <vector>
#include <string>
#include <memory>

using namespace Lumina;

namespace Tiles
{
    class Command;
    class LayerStack;
    class TileLayer;

    class PanelLayerSelection : public Panel
    {
    public:
        PanelLayerSelection(Ref<Context> context) : Panel(context) {}
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

        // Utility methods
        void ExecuteLayerCommand(std::unique_ptr<Command> command);
        bool HasWorkingLayer() const;
        bool HasLayers() const;
    };
}