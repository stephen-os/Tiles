#pragma once

#include "Lumina/Base.h"

#include "../Core/ToolModes.h"

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

        void SetToolModes(const Lumina::Ref<ToolModes>& modes) { m_ToolModes = modes; }

    private:
        Lumina::Ref<ToolModes> m_ToolModes;

        Lumina::Texture m_EraserTexture;
        Lumina::Texture m_FillTexture;

        static constexpr ImU32 SELECTION_BORDER_COLOR = IM_COL32(255, 165, 0, 255); // Orange
    };

}