#pragma once

#include "../Core/Layers.h"
#include "../Core/State.h"
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
        void SetState(const Shared<State>& state) { m_State = state; }
    private:
        Shared<Layers> m_Layers;
        Shared<State> m_State;
    };

}