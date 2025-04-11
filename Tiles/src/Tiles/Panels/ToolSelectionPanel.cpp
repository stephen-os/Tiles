#include "ToolSelectionPanel.h"

#include "../Core/Color.h"

namespace Tiles
{

    ToolSelectionPanel::ToolSelectionPanel()
    {
        std::string eraserSource = "res/assets/eraser.png";
        std::string fillSource = "res/assets/bucket.png";

        m_EraserTexture = Lumina::Texture::Create(eraserSource);
        m_FillTexture = Lumina::Texture::Create(fillSource);
    }

    void ToolSelectionPanel::Render()
    {
        ImGui::Begin("Tools");

        ImVec2 imageSize(32, 32);

        ImGui::PushID("EraserMode");

        if (ImGui::ImageButton((void*)(intptr_t)m_EraserTexture->GetID(), imageSize))
        {
            m_ToolSelection->Erase = !m_ToolSelection->Erase;
            m_TextureSelection->Clear();

            if (m_ToolSelection->Fill)
                m_ToolSelection->Fill = !m_ToolSelection->Fill;
        }

        if (m_ToolSelection->Erase)
        {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();
            ImGui::GetWindowDrawList()->AddRect(min, max, Color::SELECTION_BORDER_COLOR, 5.0f, 0, 1.0f);
        }

        ImGui::PopID();

        ImGui::SameLine();

        ImGui::PushID("FillMode");

        if (ImGui::ImageButton((void*)(intptr_t)m_FillTexture->GetID(), imageSize))
        {
            m_ToolSelection->Fill = !m_ToolSelection->Fill;

            if (m_ToolSelection->Erase)
                m_ToolSelection->Erase = !m_ToolSelection->Erase;
        }

        if (m_ToolSelection->Fill)
        {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();
            ImGui::GetWindowDrawList()->AddRect(min, max, Color::SELECTION_BORDER_COLOR, 5.0f, 0, 1.0f);
        }

        ImGui::PopID();

        ImGui::End();

        if (m_ToolSelection->Erase)
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            ImVec2 mousePos = ImGui::GetMousePos();

            ImGui::GetForegroundDrawList()->AddImage(
                (void*)(intptr_t)m_EraserTexture->GetID(),
                ImVec2(mousePos.x - imageSize.x / 2, mousePos.y - imageSize.y / 2),
                ImVec2(mousePos.x + imageSize.x / 2, mousePos.y + imageSize.y / 2)
            );
        }

        if (m_ToolSelection->Fill)
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            ImVec2 mousePos = ImGui::GetMousePos();

            ImGui::GetForegroundDrawList()->AddImage(
                (void*)(intptr_t)m_FillTexture->GetID(),
                ImVec2(mousePos.x - imageSize.x / 2, mousePos.y - imageSize.y / 2),
                ImVec2(mousePos.x + imageSize.x / 2, mousePos.y + imageSize.y / 2)
            );
        }
    }

}