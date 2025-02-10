#pragma once

#include "../Core/TileLayer.h"
#include "Lumina/Renderer/TextureAtlas.h"

class AttributePanel
{
public:
    void Render();

    // Set the TileLayer pointer
    void SetTileLayer(TileLayer& layers) { m_TileLayer = &layers; }

    // Set the TextureSelectionPanel pointer
    void SetTextureAtlas(Lumina::TextureAtlas& atlas) { m_TextureAtlas = &atlas; }

private:
    TileLayer* m_TileLayer = nullptr;
    Lumina::TextureAtlas* m_TextureAtlas = nullptr;
};
