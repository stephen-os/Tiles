#pragma once

#include "../Core/Layers.h"

#include "Lumina/Base.h"

class LayerSelectionPanel
{
public:
    void Render();

    // Set the TileLayer pointer
    void SetTileLayer(const Lumina::Ref<Layers>& layers) { m_Layers = layers; }
private:
    Lumina::Ref<Layers> m_Layers;
};