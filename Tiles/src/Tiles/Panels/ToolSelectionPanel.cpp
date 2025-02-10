#include "ToolSelectionPanel.h"

#include <imgui.h>

void ToolSelectionPanel::Render()
{
    if (!m_Modes) return;

    ImGui::Begin("Tools");

    ImGui::Checkbox("Eraser Mode", &m_Modes->Erase);
    ImGui::Checkbox("Fill Mode", &m_Modes->Fill);

    ImGui::End();
}
