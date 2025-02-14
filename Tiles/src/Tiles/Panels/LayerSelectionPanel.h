#pragma once

#include "../Core/Layers.h"
#include "../Core/State.h"

#include "Lumina/Base.h"

class LayerSelectionPanel
{
public:
    void Render();

    // Set the TileLayer pointer
    void SetTileLayer(const Lumina::Ref<Layers>& layers) { m_Layers = layers; }
    void SetState(const Lumina::Ref<State>& state) { m_State = state; }
private:
    Lumina::Ref<Layers> m_Layers;
    Lumina::Ref<State> m_State;
};