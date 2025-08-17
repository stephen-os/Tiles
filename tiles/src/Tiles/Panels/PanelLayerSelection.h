#pragma once

#include "Panel.h"

namespace Tiles
{
    class PanelLayerSelection : public Panel
    {
    public:
        PanelLayerSelection() = default;
        ~PanelLayerSelection() = default;
        
        void Render() override;
		void Update() override;

    private:
        void RenderProjectInfo();
        void RenderSelectedLayerInfo();
        
        void RenderLayerList();
        void RenderLayerControls();
        void RenderLayerItem(size_t layerIndex, LayerStack& layerStack);
        void RenderLayerProperties(const TileLayer& layer);
        void RenderLayerMovementControls();
        void RenderLayerOperationControls();

        void ExecuteLayerCommand(std::unique_ptr<Command> command);
    };
}
