#pragma once

#include "../Core/Layers.h"
#include "../Core/Base.h"

namespace Tiles
{

    class LayerSelectionPanel
    {
    public:
        LayerSelectionPanel() = default;
		~LayerSelectionPanel() = default;

        void OnUIRender();

        // Set the TileLayer pointer
        void SetLayers(const Shared<Layers>& layers) { m_Layers = layers; }
    private:
        Shared<Layers> m_Layers;
    };

}