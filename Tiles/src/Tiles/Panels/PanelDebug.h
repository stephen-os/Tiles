#pragma once
#include "Panel.h"
#include "Lumina/Lumina.h"
#include "imgui.h"

using namespace Lumina;

namespace Tiles
{
    class PanelDebug : public Panel
    {
    public:
        PanelDebug(Ref<Context> context);
        ~PanelDebug() = default;

        void Render() override;
        void Update() override;

    private:
        // Helper methods to render different sections
        void RenderContextInfo();
        void RenderProjectInfo();
        void RenderLayerStackInfo();
        void RenderWorkingLayerInfo();
        void RenderPaintingInfo();
        void RenderCommandHistoryInfo();
        void RenderTextureAtlasInfo();
        void RenderPerformanceInfo();

        // Helper to format time points
        std::string FormatTimePoint(const std::chrono::steady_clock::time_point& timePoint) const;

        // Helper to get painting mode string
        const char* GetPaintingModeString(PaintingMode mode) const;

        // Performance tracking
        float m_UpdateTime = 0.0f;
        float m_RenderTime = 0.0f;
        std::chrono::steady_clock::time_point m_LastUpdateTime;
    };
}