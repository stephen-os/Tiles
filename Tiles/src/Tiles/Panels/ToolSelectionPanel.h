#pragma once

#include "../Core/ToolModes.h"

class ToolSelectionPanel
{
public:
    void Render();

    void SetToolModes(ToolModes& modes) { m_Modes = &modes; }

private:
    ToolModes* m_Modes = nullptr;
};