#include "PanelTileAttributes.h"
#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include <algorithm>

namespace Tiles
{
    // UI layout constants
    namespace UILayout
    {
        static constexpr float MIN_SLIDER_WIDTH = 150.0f;
        static constexpr float SLIDER_WIDTH_OFFSET = 120.0f;
        static constexpr float LABEL_COLUMN_WIDTH = 30.0f;
        static constexpr float QUICK_BUTTON_WIDTH = 40.0f;
        static constexpr float WIDE_BUTTON_WIDTH = 45.0f;
        static constexpr float PRESET_BUTTON_WIDTH = 50.0f;
    }

    // Value range constants
    namespace ValueRanges
    {
        static constexpr float MIN_ROTATION = -180.0f;
        static constexpr float MAX_ROTATION = 180.0f;
        static constexpr float MIN_SIZE = 0.1f;
        static constexpr float MAX_SIZE = 5.0f;
    }

    // Preset values
    namespace PresetValues
    {
        namespace Rotation
        {
            static constexpr float DEGREES_0 = 0.0f;
            static constexpr float DEGREES_90 = 90.0f;
            static constexpr float DEGREES_180 = 180.0f;
            static constexpr float DEGREES_270 = 270.0f;
        }

        namespace Size
        {
            static constexpr float HALF = 0.5f;
            static constexpr float NORMAL = 1.0f;
            static constexpr float DOUBLE = 2.0f;
            static constexpr float TRIPLE = 3.0f;
        }

        namespace Tint
        {
            static const glm::vec4 WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
            static const glm::vec4 RED = { 1.0f, 0.3f, 0.3f, 1.0f };
            static const glm::vec4 GREEN = { 0.3f, 1.0f, 0.3f, 1.0f };
            static const glm::vec4 BLUE = { 0.3f, 0.3f, 1.0f, 1.0f };
        }
    }

    // Color scheme for UI elements
    namespace ColorScheme
    {
        // Section header colors
        namespace Headers
        {
            static const ImVec4 ROTATION = { 1.0f, 0.8f, 0.4f, 1.0f };
            static const ImVec4 SIZE = { 0.4f, 0.8f, 1.0f, 1.0f };
            static const ImVec4 TINT = { 0.8f, 0.4f, 0.8f, 1.0f };
            static const ImVec4 RESET = { 0.9f, 0.9f, 0.9f, 1.0f };
        }

        // Status and hint colors
        namespace Status
        {
            static const ImVec4 HINT_TEXT = { 0.7f, 0.7f, 0.7f, 1.0f };
            static const ImVec4 NO_PROJECT = { 0.7f, 0.7f, 0.7f, 1.0f };
        }

        // Button color schemes
        namespace Buttons
        {
            static const ImVec4 NORMAL = { 0.2f, 0.4f, 0.6f, 1.0f };
            static const ImVec4 NORMAL_HOVERED = { 0.3f, 0.5f, 0.7f, 1.0f };
            static const ImVec4 NORMAL_ACTIVE = { 0.1f, 0.3f, 0.5f, 1.0f };

            static const ImVec4 RESET_ALL = { 0.8f, 0.3f, 0.3f, 1.0f };
            static const ImVec4 RESET_ALL_HOVERED = { 0.9f, 0.4f, 0.4f, 1.0f };
            static const ImVec4 RESET_ALL_ACTIVE = { 0.7f, 0.2f, 0.2f, 1.0f };
        }

        // Slider color schemes
        namespace Sliders
        {
            static const ImVec4 GRAB = { 0.3f, 0.6f, 0.9f, 1.0f };
            static const ImVec4 GRAB_ACTIVE = { 0.4f, 0.7f, 1.0f, 1.0f };
            static const ImVec4 FRAME_BG = { 0.2f, 0.2f, 0.2f, 1.0f };
            static const ImVec4 FRAME_BG_HOVERED = { 0.3f, 0.3f, 0.3f, 1.0f };
            static const ImVec4 FRAME_BG_ACTIVE = { 0.4f, 0.4f, 0.4f, 1.0f };
        }
    }

    void PanelTileAttributes::Render()
    {
        ImGui::Begin("Tile Attributes", nullptr, ImGuiWindowFlags_NoScrollWithMouse);

        if (!m_Context)
        {
            ImGui::TextColored(ColorScheme::Status::NO_PROJECT, "No project loaded");
            ImGui::End();
            return;
        }

        Tile& brush = m_Context->GetBrush();

        // Update temp values from brush
        m_TempRotation = brush.GetRotation();
        m_TempSize = brush.GetSize();
        m_TempTint = brush.GetTint();

        // Calculate responsive slider width
        float windowWidth = ImGui::GetContentRegionAvail().x;
        m_SliderWidth = std::max(UILayout::MIN_SLIDER_WIDTH, windowWidth - UILayout::SLIDER_WIDTH_OFFSET);

        RenderSeparator();
        RenderRotationControls();
        RenderSeparator();
        RenderSizeControls();
        RenderSeparator();
        RenderTintControls();
        RenderSeparator();
        RenderResetButtons();

        // Apply changes back to brush
        brush.SetRotation(m_TempRotation);
        brush.SetSize(m_TempSize);
        brush.SetTint(m_TempTint);

        ImGui::End();
    }

    void PanelTileAttributes::Update()
    {
        // This panel does not require any specific update logic
	}

    void PanelTileAttributes::RenderSeparator()
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }

    void PanelTileAttributes::RenderRotationControls()
    {
        RenderSectionHeader("Rotation", ColorScheme::Headers::ROTATION);

        RenderRotationSlider("X:", m_TempRotation.x);
        RenderRotationSlider("Y:", m_TempRotation.y);
        RenderRotationSlider("Z:", m_TempRotation.z);

        RenderQuickRotationButtons();
    }

    void PanelTileAttributes::RenderSectionHeader(const char* title, const ImVec4& color)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Text("%s", title);
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }

    void PanelTileAttributes::RenderRotationSlider(const char* label, float& value)
    {
        ImGui::Text("%s", label);
        ImGui::SameLine(UILayout::LABEL_COLUMN_WIDTH);
        ImGui::SetNextItemWidth(m_SliderWidth);

        RenderStyledSlider(value, ValueRanges::MIN_ROTATION, ValueRanges::MAX_ROTATION, "%.1f\u00B0");
    }

    void PanelTileAttributes::RenderStyledSlider(float& value, float min, float max, const char* format)
    {
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ColorScheme::Sliders::GRAB);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ColorScheme::Sliders::GRAB_ACTIVE);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ColorScheme::Sliders::FRAME_BG);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ColorScheme::Sliders::FRAME_BG_HOVERED);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ColorScheme::Sliders::FRAME_BG_ACTIVE);

        // Use memory address for unique slider ID
        char sliderIdStr[32];
        snprintf(sliderIdStr, sizeof(sliderIdStr), "##Slider%p", (void*)&value);

        ImGui::SliderFloat(sliderIdStr, &value, min, max, format);

        ImGui::PopStyleColor(5);
    }

    void PanelTileAttributes::RenderQuickRotationButtons()
    {
        ImGui::Spacing();

        if (ImGui::Button("0\u00B0", ImVec2(UILayout::QUICK_BUTTON_WIDTH, 0)))
            m_TempRotation.z = PresetValues::Rotation::DEGREES_0;
        ImGui::SameLine();

        if (ImGui::Button("90\u00B0", ImVec2(UILayout::QUICK_BUTTON_WIDTH, 0)))
            m_TempRotation.z = PresetValues::Rotation::DEGREES_90;
        ImGui::SameLine();

        if (ImGui::Button("180\u00B0", ImVec2(UILayout::WIDE_BUTTON_WIDTH, 0)))
            m_TempRotation.z = PresetValues::Rotation::DEGREES_180;
        ImGui::SameLine();

        if (ImGui::Button("270\u00B0", ImVec2(UILayout::WIDE_BUTTON_WIDTH, 0)))
            m_TempRotation.z = PresetValues::Rotation::DEGREES_270;
    }

    void PanelTileAttributes::RenderSizeControls()
    {
        RenderSectionHeader("Size", ColorScheme::Headers::SIZE);

        RenderLinkCheckbox();
        RenderSizeSliders();
        RenderQuickSizeButtons();
    }

    void PanelTileAttributes::RenderLinkCheckbox()
    {
        if (ImGui::Checkbox("Link", &m_LinkSizeComponents))
        {
            if (m_LinkSizeComponents)
            {
                float avgSize = (m_TempSize.x + m_TempSize.y) * 0.5f;
                m_TempSize.x = m_TempSize.y = avgSize;
            }
        }
        ImGui::SameLine();
        ImGui::TextColored(ColorScheme::Status::HINT_TEXT, "(Proportional)");
    }

    void PanelTileAttributes::RenderSizeSliders()
    {
        // Width slider
        ImGui::Text("W:");
        ImGui::SameLine(UILayout::LABEL_COLUMN_WIDTH);
        ImGui::SetNextItemWidth(m_SliderWidth);

        float oldWidth = m_TempSize.x;
        RenderStyledSlider(m_TempSize.x, ValueRanges::MIN_SIZE, ValueRanges::MAX_SIZE, "%.2f");
        bool widthChanged = (oldWidth != m_TempSize.x);

        // Apply proportional scaling if linked
        if (m_LinkSizeComponents && widthChanged && oldWidth != 0.0f)
        {
            float ratio = m_TempSize.x / oldWidth;
            m_TempSize.y *= ratio;
        }

        // Height slider
        ImGui::Text("H:");
        ImGui::SameLine(UILayout::LABEL_COLUMN_WIDTH);
        ImGui::SetNextItemWidth(m_SliderWidth);

        float oldHeight = m_TempSize.y;
        RenderStyledSlider(m_TempSize.y, ValueRanges::MIN_SIZE, ValueRanges::MAX_SIZE, "%.2f");
        bool heightChanged = (oldHeight != m_TempSize.y);

        // Apply proportional scaling if linked
        if (m_LinkSizeComponents && heightChanged && oldHeight != 0.0f)
        {
            float ratio = m_TempSize.y / oldHeight;
            m_TempSize.x *= ratio;
        }
    }

    void PanelTileAttributes::RenderQuickSizeButtons()
    {
        ImGui::Spacing();

        if (ImGui::Button("0.5x", ImVec2(UILayout::QUICK_BUTTON_WIDTH, 0)))
            m_TempSize = glm::vec2(PresetValues::Size::HALF, PresetValues::Size::HALF);
        ImGui::SameLine();

        if (ImGui::Button("1x", ImVec2(UILayout::QUICK_BUTTON_WIDTH, 0)))
            m_TempSize = glm::vec2(PresetValues::Size::NORMAL, PresetValues::Size::NORMAL);
        ImGui::SameLine();

        if (ImGui::Button("2x", ImVec2(UILayout::QUICK_BUTTON_WIDTH, 0)))
            m_TempSize = glm::vec2(PresetValues::Size::DOUBLE, PresetValues::Size::DOUBLE);
        ImGui::SameLine();

        if (ImGui::Button("3x", ImVec2(UILayout::QUICK_BUTTON_WIDTH, 0)))
            m_TempSize = glm::vec2(PresetValues::Size::TRIPLE, PresetValues::Size::TRIPLE);
    }

    void PanelTileAttributes::RenderTintControls()
    {
        RenderSectionHeader("Tint", ColorScheme::Headers::TINT);

        // Color picker
        ImGui::SetNextItemWidth(m_SliderWidth);
        ImGui::ColorEdit4("##Tint", glm::value_ptr(m_TempTint),
            ImGuiColorEditFlags_AlphaBar |
            ImGuiColorEditFlags_AlphaPreview |
            ImGuiColorEditFlags_DisplayRGB);

        RenderTintPresets();
    }

    void PanelTileAttributes::RenderTintPresets()
    {
        ImGui::Spacing();

        if (ImGui::Button("White", ImVec2(UILayout::PRESET_BUTTON_WIDTH, 0)))
            m_TempTint = PresetValues::Tint::WHITE;
        ImGui::SameLine();

        if (ImGui::Button("Red", ImVec2(UILayout::QUICK_BUTTON_WIDTH, 0)))
            m_TempTint = PresetValues::Tint::RED;
        ImGui::SameLine();

        if (ImGui::Button("Green", ImVec2(UILayout::PRESET_BUTTON_WIDTH, 0)))
            m_TempTint = PresetValues::Tint::GREEN;
        ImGui::SameLine();

        if (ImGui::Button("Blue", ImVec2(UILayout::QUICK_BUTTON_WIDTH, 0)))
            m_TempTint = PresetValues::Tint::BLUE;
    }

    void PanelTileAttributes::RenderResetButtons()
    {
        RenderSectionHeader("Reset", ColorScheme::Headers::RESET);

        float buttonWidth = CalculateResetButtonWidth();

        RenderIndividualResetButtons(buttonWidth);
        ImGui::Spacing();
        RenderResetAllButton();
    }

    float PanelTileAttributes::CalculateResetButtonWidth()
    {
        return (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x * 2) / 3.0f;
    }

    void PanelTileAttributes::RenderIndividualResetButtons(float buttonWidth)
    {
        RenderStyledButton("Reset Rotation", ImVec2(buttonWidth, 0), [this]() {
            m_TempRotation = glm::vec3(0.0f, 0.0f, 0.0f);
            });

        ImGui::SameLine();
        RenderStyledButton("Reset Size", ImVec2(buttonWidth, 0), [this]() {
            m_TempSize = glm::vec2(PresetValues::Size::NORMAL, PresetValues::Size::NORMAL);
            });

        ImGui::SameLine();
        RenderStyledButton("Reset Tint", ImVec2(buttonWidth, 0), [this]() {
            m_TempTint = PresetValues::Tint::WHITE;
            });
    }

    void PanelTileAttributes::RenderStyledButton(const char* label, const ImVec2& size, std::function<void()> onClick)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ColorScheme::Buttons::NORMAL);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorScheme::Buttons::NORMAL_HOVERED);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorScheme::Buttons::NORMAL_ACTIVE);

        if (ImGui::Button(label, size))
        {
            onClick();
        }

        ImGui::PopStyleColor(3);
    }

    void PanelTileAttributes::RenderResetAllButton()
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ColorScheme::Buttons::RESET_ALL);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorScheme::Buttons::RESET_ALL_HOVERED);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorScheme::Buttons::RESET_ALL_ACTIVE);

        if (ImGui::Button("Reset All", ImVec2(-1, 0)))
        {
            m_TempRotation = glm::vec3(0.0f, 0.0f, 0.0f);
            m_TempSize = glm::vec2(PresetValues::Size::NORMAL, PresetValues::Size::NORMAL);
            m_TempTint = PresetValues::Tint::WHITE;
        }

        ImGui::PopStyleColor(3);
    }
}