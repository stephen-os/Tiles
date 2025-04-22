#pragma once

#include "../Core/TileAttributes.h"
#include "../Core/Modes.h"

#include "Lumina/Core/Aliases.h"

#include "Lumina/Renderer/Texture.h"


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
        void RenderToolButton(const char* id, const Shared<Lumina::Texture>& texture, Modes::Mode mode);
        void DrawCursorForMode(Modes::Mode mode, const Shared<Lumina::Texture>& texture);
    private:
        Shared<TileAttributes> m_TileAttributes;

        Shared<Lumina::Texture> m_EraserTexture;
        Shared<Lumina::Texture> m_FillTexture;
    };
}