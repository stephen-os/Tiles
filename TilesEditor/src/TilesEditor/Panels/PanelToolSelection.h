#pragma once

#include "Panel.h"


#include "Tiles.h"

#include "imgui.h"

#include <vector>
#include <string>

namespace Tiles
{
    /// Tool palette for the three painting modes (Brush/Eraser/Fill). The active
    /// tool is stored on the Context as its PaintingMode. Also draws a texture as
    /// a custom mouse cursor while a tool is active.
    class PanelToolSelection : public Panel
    {
    public:
        PanelToolSelection(std::shared_ptr<Context> context);
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
        void RenderComponentToolButton(const char* id, ToolType toolType, const std::shared_ptr<Tiles::Texture>& texture, PaintingMode mode, const char* tooltip);
        void RenderComponentCursorForMode(const char* id, PaintingMode mode, const std::shared_ptr<Tiles::Texture>& texture);
        void LoadTextures();
        bool IsToolSelected(PaintingMode mode) const;
        void SetToolSelection(PaintingMode mode);

    private:
        std::shared_ptr<Tiles::Texture> m_BrushTexture = nullptr;
        std::shared_ptr<Tiles::Texture> m_EraserTexture = nullptr;
        std::shared_ptr<Tiles::Texture> m_FillTexture = nullptr;
    };
}