#pragma once

#include "Panel.h"
#include "glm/glm.hpp"
#include "imgui.h"
#include <string>

namespace Tiles
{
    class PanelBrushAttributes : public Panel
    {
    public:
        PanelBrushAttributes(Ref<Context> context) : Panel(context) {}
        ~PanelBrushAttributes() = default;

        void Render() override;
        void Update() override;

    private:
        void RenderBlockBrushAttributes();

        void RenderSectionRotation();
        void RenderSectionSize();
        void RenderSectionTint();
        void RenderSectionReset();

        void RenderComponentTitle(const char* title);
        void RenderComponentVec2Controls(const char* id, glm::vec2& vec, float minVal, float maxVal, const char* format = "%.3f", const char* xName = "X", const char* yName = "Y");
        void RenderComponentVec3Controls(const char* id, glm::vec3& vec, float minVal, float maxVal, const char* format = "%.3f", const char* xName = "X", const char* yName = "Y", const char* zName = "Z");
        void RenderComponentVec4Controls(const char* id, glm::vec4& vec, float minVal, float maxVal, const char* format = "%.3f", const char* xName = "X", const char* yName = "Y", const char* zName = "Z", const char* wName = "W");
        void RenderComponentLabel(const char* id, const char* label, const ImVec4& color);
        void RenderComponentDragFloat(const char* id, float* value, float speed, float minVal, float maxVal, const char* format = "%.3f");
        void RenderComponentColorPicker(const char* id, glm::vec4& color);
        bool RenderComponentButton(const char* id, const char* label, const ImVec4& buttonColor);
    };
}