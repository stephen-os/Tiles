#pragma once

#include "../Core/TileLayer.h"

#include "Lumina/Renderer/TextureAtlas.h"
#include "Lumina/Base.h"

class AttributePanel
{
public:
    void Render();

    // Set the TileLayer pointer
    void SetTileLayer(const Lumina::Ref<TileLayer>& layers) { m_TileLayer = layers; }

    // Set the TextureSelectionPanel pointer
    void SetTextureAtlas(Lumina::TextureAtlas& atlas) { m_TextureAtlas = &atlas; }

private:
    Lumina::Ref<TileLayer> m_TileLayer;
    Lumina::TextureAtlas* m_TextureAtlas = nullptr;
};
