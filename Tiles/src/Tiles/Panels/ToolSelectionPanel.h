#pragma once

#include "Lumina/Base.h"

#include "../Core/ToolModes.h"

class ToolSelectionPanel
{
public:
    void Render();

    void SetToolModes(const Lumina::Ref<ToolModes>& modes) { m_ToolModes = modes; }

private:
    Lumina::Ref<ToolModes> m_ToolModes;
};