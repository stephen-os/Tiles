#pragma once

#include "../Core/TileAttributes.h"

#include "Lumina/Core/Aliases.h"

#include "Lumina/Renderer/Texture.h"

#include "imgui.h"

namespace Tiles
{

    class ToolSelectionPanel
    {
    public:
        ToolSelectionPanel();
        ~ToolSelectionPanel() = default;

        void Render();

        void SetTileAttributes(const Shared<TileAttributes>& tileAttributes) { m_TileAttributes = tileAttributes; };

    private:
        Shared<TileAttributes> m_TileAttributes;

        Shared<Lumina::Texture> m_EraserTexture;
        Shared<Lumina::Texture> m_FillTexture;
    };

}