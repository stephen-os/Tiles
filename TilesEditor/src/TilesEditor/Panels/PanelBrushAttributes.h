#pragma once

#include "Panel.h"
#include "glm/glm.hpp"
#include "imgui.h"
#include <string>

namespace Tiles::Editor
{
    // Editor for the current brush's rotation, size, and tint. Controls read and
    // write the Session's brush directly, with drag fields, preset buttons, and a
    // colour picker; changes take effect immediately (no undo step).
    class PanelBrushAttributes : public Panel
    {
    public:
        PanelBrushAttributes(EditorHost& host) : Panel(host) {}
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

        // Render one coloured-label + drag-field per vector component, editing
        // `vec` in place. The X/Y/Z/W names label the columns for non-spatial
        // vectors (e.g. W/H for size, R/G/B/A for tint).
        void RenderComponentVec2Controls(const char* id, glm::vec2& vec, float minVal, float maxVal, const char* format = "%.3f", const char* xName = "X", const char* yName = "Y");
        void RenderComponentVec3Controls(const char* id, glm::vec3& vec, float minVal, float maxVal, const char* format = "%.3f", const char* xName = "X", const char* yName = "Y", const char* zName = "Z");
        void RenderComponentVec4Controls(const char* id, glm::vec4& vec, float minVal, float maxVal, const char* format = "%.3f", const char* xName = "X", const char* yName = "Y", const char* zName = "Z", const char* wName = "W");
        void RenderComponentLabel(const char* id, const char* label, const ImVec4& color);
        void RenderComponentDragFloat(const char* id, float* value, float speed, float minVal, float maxVal, const char* format = "%.3f");
        void RenderComponentColorPicker(const char* id, glm::vec4& color);
        bool RenderComponentButton(const char* id, const char* label, const ImVec4& buttonColor);
    };
}