#pragma once

#include "../Core/TileLayer.h"
#include "Lumina/Base.h"

class LayerSelectionPanel
{
public:
    void Render();

    // Set the TileLayer pointer
    void SetTileLayer(const Lumina::Ref<TileLayer>& layers) { m_TileLayer = layers; }
private:
    Lumina::Ref<TileLayer> m_TileLayer;
};