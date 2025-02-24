#pragma once

#include "../Core/Layers.h"
#include "../Core/Base.h"
#include "../Commands/CommandHistory.h"

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
        void SetCommandHistory(const Shared<CommandHistory>& history) { m_CommandHistory = history; }
    private:
        Shared<Layers> m_Layers;
        Shared<CommandHistory> m_CommandHistory;
    };

}