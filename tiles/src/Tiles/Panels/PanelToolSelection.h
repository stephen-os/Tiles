#pragma once

#include "Panel.h"


#include "Lumina/Lumina.h"

#include "imgui.h"

#include <vector>
#include <string>

using namespace Lumina;

namespace Tiles
{
    class PanelToolSelection : public Panel
    {
    public:
        PanelToolSelection();
        ~PanelToolSelection() = default;

        void Render() override;
        void Update() override;

    private:
        enum class ToolType
        {
            Brush,
            Eraser,
            Fill
        };

        void RenderBlockToolButtons();
        void RenderBlockCustomCursor();
        void RenderComponentToolButton(const char* id, ToolType toolType, const Ref<Lumina::Texture>& texture, PaintingMode mode, const char* tooltip);
        void RenderComponentCursorForMode(const char* id, PaintingMode mode, const Ref<Lumina::Texture>& texture);
        void LoadTextures();
        bool IsToolSelected(PaintingMode mode) const;
        void SetToolSelection(PaintingMode mode);

    private:
        Ref<Lumina::Texture> m_BrushTexture = nullptr;
        Ref<Lumina::Texture> m_EraserTexture = nullptr;
        Ref<Lumina::Texture> m_FillTexture = nullptr;
    };
}