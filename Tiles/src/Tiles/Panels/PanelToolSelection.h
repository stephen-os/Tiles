#pragma once

#include "Panel.h"


#include "../Lumina.h"

#include "imgui.h"

#include <vector>
#include <string>

using namespace Tiles;

namespace Tiles
{
    class PanelToolSelection : public Panel
    {
    public:
        PanelToolSelection(Ref<Context> context);
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
        void RenderComponentToolButton(const char* id, ToolType toolType, const Ref<Tiles::Texture>& texture, PaintingMode mode, const char* tooltip);
        void RenderComponentCursorForMode(const char* id, PaintingMode mode, const Ref<Tiles::Texture>& texture);
        void LoadTextures();
        bool IsToolSelected(PaintingMode mode) const;
        void SetToolSelection(PaintingMode mode);

    private:
        Ref<Tiles::Texture> m_BrushTexture = nullptr;
        Ref<Tiles::Texture> m_EraserTexture = nullptr;
        Ref<Tiles::Texture> m_FillTexture = nullptr;
    };
}