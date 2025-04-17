#pragma once

#include "../Core/TextureSelection.h"

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

        void SetTextureSelection(const Shared<TextureSelection>& textureSelection) { m_TextureSelection = textureSelection; };

    private:
        Shared<TextureSelection> m_TextureSelection;

        Shared<Lumina::Texture> m_EraserTexture;
        Shared<Lumina::Texture> m_FillTexture;
    };

}