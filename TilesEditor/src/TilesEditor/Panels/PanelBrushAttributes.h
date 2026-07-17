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
    };
}