#pragma once

#include "../Core/TileLayer.h"

class LayerSelectionPanel
{
public:
    void Render();

    // Set the TileLayer pointer
    void SetTileLayer(TileLayer& layers) { m_TileLayer = &layers; }
private:
    TileLayer* m_TileLayer = nullptr; 
};