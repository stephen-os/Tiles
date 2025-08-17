#pragma once

#include "Panel.h"

#include "glm/glm.hpp"

#include "imgui.h"

#include <functional>

namespace Tiles
{
    class PanelTileAttributes : public Panel
    {
    public:
        PanelTileAttributes() = default;
        ~PanelTileAttributes() = default;

        void Render() override;
		void Update() override;

    private:
        void RenderRotationControls();
        void RenderSizeControls();
        void RenderTintControls();
        void RenderResetButtons();

        void RenderSeparator();
        void RenderSectionHeader(const char* title, const ImVec4& color);
        void RenderRotationSlider(const char* label, float& value);
        void RenderStyledSlider(float& value, float min, float max, const char* format);
        void RenderQuickRotationButtons();
        void RenderLinkCheckbox();
        void RenderSizeSliders();
        void RenderQuickSizeButtons();
        void RenderTintPresets();
        void RenderIndividualResetButtons(float buttonWidth);
        void RenderStyledButton(const char* label, const ImVec2& size, std::function<void()> onClick);
        void RenderResetAllButton();

        float CalculateResetButtonWidth();
    
    private: 
        float m_SliderWidth = 200.0f;
        bool m_LinkSizeComponents = true;
        glm::vec3 m_TempRotation = { 0.0f, 0.0f, 0.0f };
        glm::vec2 m_TempSize = { 1.0f, 1.0f };
        glm::vec4 m_TempTint = { 1.0f, 1.0f, 1.0f, 1.0f };
    };
}