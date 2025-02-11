#include "ToolSelectionPanel.h"

#include <imgui.h>

void ToolSelectionPanel::Render()
{
    if (!m_ToolModes) return;

    ImGui::Begin("Tools");

    ImGui::Checkbox("Eraser Mode", &m_ToolModes->Erase);
    ImGui::Checkbox("Fill Mode", &m_ToolModes->Fill);

    ImGui::End();
}
