#pragma once

#include "Panel.h"

#include "Lumina/Graphics/Cameras/OrthographicCamera.h"

#include "Lumina/Graphics/RenderTarget.h"

#include "imgui.h"

namespace Tiles
{
    class PanelBrushPreview : public Panel
    {
    public:
        PanelBrushPreview();
        ~PanelBrushPreview() = default;
        
        void Render() override;
		void Update() override; 

    private:
        void RenderPreviewControls();
        void RenderBackground();
        void RenderGrid();
        void RenderBrush(const Tile& brush);
        void RenderBounds(const Tile& brush);
        void RenderBrushInfo();
        void HandleMouseInteraction(const ImVec2& previewSize);

    private:
        OrthographicCamera m_Camera;
        Ref<RenderTarget> m_PreviewRenderTarget;

        bool m_ShowGrid = true;
        bool m_ShowBounds = false;
        float m_BackgroundBrightness = 0.2f;
        float m_ZoomLevel = 2.0f;
        glm::vec2 m_PanOffset = { 0.0f, 0.0f };
    };
}