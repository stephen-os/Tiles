#pragma once

#include "../Core/ToolSelection.h"
#include "../Core/Base.h"

#include "Lumina/Renderer/Texture.h"
#include "Lumina/Base.h"

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

    private:
        Shared<ToolSelection> m_ToolSelection;

        Lumina::Texture m_EraserTexture;
        Lumina::Texture m_FillTexture;
    };

}