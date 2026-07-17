#pragma once

#include "Panel.h"

#include "Tiles.h"

#include "imgui.h"

#include <vector>
#include <string>

namespace Tiles::Editor
{
    // Read-only preview of the current brush. Renders the brush quad with its own
    // camera into an offscreen target (pan by dragging, zoom with the wheel) and
    // lists the brush's rotation/size/tint/UVs/atlas below it.
    class PanelBrushPreview : public Panel
    {
    public:
        PanelBrushPreview(EditorHost& host);
        ~PanelBrushPreview() = default;

        void Render() override;
        void Update() override;

    private:
        void RenderBlockViewport();
        void RenderBlockBrushInfo();

        void RenderSectionRotation();
        void RenderSectionSize();
        void RenderSectionTint();
        void RenderSectionUVs();
        void RenderSectionAtlas();

    private:
        std::shared_ptr<Tiles::RenderTarget> m_PreviewRenderTarget;

        bool m_ShowGrid = true;
        bool m_ShowBounds = false;
        float m_BackgroundBrightness = 0.2f;
        float m_ZoomLevel = 2.0f;
        glm::vec2 m_PanOffset = { 0.0f, 0.0f };
    };
}