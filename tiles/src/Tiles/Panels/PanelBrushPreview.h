#pragma once

#include "Panel.h"

#include "Lumina/Lumina.h"

#include "imgui.h"

#include <vector>
#include <string>

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
        void RenderBlockViewport();
        void RenderBlockBrushInfo();

        void RenderSectionRotation();
        void RenderSectionSize();
        void RenderSectionTint();
        void RenderSectionUVs();
        void RenderSectionAtlas();

        void RenderComponentTitle(const char* title);
        void RenderComponentVec2Table(const char* id, const glm::vec2& vec, const char* format = "%.3f", const char* xName = "X", const char* yName = "Y");
        void RenderComponentVec3Table(const char* id, const glm::vec3& vec, const char* format = "%.3f", const char* xName = "X", const char* yName = "Y", const char* zName = "Z");
        void RenderComponentVec4Table(const char* id, const glm::vec4& vec, const char* format = "%.3f", const char* xName = "X", const char* yName = "Y", const char* zName = "Z", const char* wName = "W");
        void RenderComponentLabel(const char* id, const char* label, const ImVec4& color);
        void RenderComponentValue(const char* id, float value, const char* format = "%.3f");
        void RenderComponentColorSwatch(const char* id, const glm::vec4& color);

        void RenderComponentSpacing();

    private:
        Lumina::Ref<Lumina::OrthographicCamera> m_Camera;
        Lumina::Ref<Lumina::RenderTarget> m_PreviewRenderTarget;

        bool m_ShowGrid = true;
        bool m_ShowBounds = false;
        float m_BackgroundBrightness = 0.2f;
        float m_ZoomLevel = 2.0f;
        glm::vec2 m_PanOffset = { 0.0f, 0.0f };
    };
}