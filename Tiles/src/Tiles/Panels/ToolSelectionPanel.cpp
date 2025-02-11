#include "ToolSelectionPanel.h"

ToolSelectionPanel::ToolSelectionPanel()
{
    m_EraserTexture.SetData("res/assets/eraser.png");
}

void ToolSelectionPanel::Render()
{
    if (!m_ToolModes) return;

    ImGui::Begin("Tools");

    ImGui::PushID("EraserMode");

    ImVec2 imageSize(32, 32);

    if (ImGui::ImageButton((void*)(intptr_t)m_EraserTexture.GetID(), imageSize))
    {
        m_ToolModes->Erase = !m_ToolModes->Erase;
    }

    if (m_ToolModes->Erase)
    {
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        ImGui::GetWindowDrawList()->AddRect(min, max, SELECTION_BORDER_COLOR, 5.0f, 0, 1.0f);
    }

    ImGui::PopID();


    ImGui::Checkbox("Fill Mode", &m_ToolModes->Fill);

    ImGui::End();
}
