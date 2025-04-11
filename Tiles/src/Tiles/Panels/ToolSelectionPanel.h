#pragma once

#include "../Core/ToolSelection.h"
#include "../Core/TextureSelection.h"
#include "../Core/Base.h"

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

        void SetToolSelection(const Shared<ToolSelection>& toolSelection) { m_ToolSelection = toolSelection; }
        void SetTextureSelection(const Shared<TextureSelection>& textureSelection) { m_TextureSelection = textureSelection; };

    private:
        Shared<ToolSelection> m_ToolSelection;
        Shared<TextureSelection> m_TextureSelection;

        Shared<Lumina::Texture> m_EraserTexture;
        Shared<Lumina::Texture> m_FillTexture;
    };

}