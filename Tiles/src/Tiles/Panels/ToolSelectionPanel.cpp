#include "ToolSelectionPanel.h"

#include "../Core/Color.h"
#include "../Core/Selection.h"

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
            if (Selection::GetCurrentMode() == Selection::Mode::Erase)
            {
                if (m_TileAttributes->GetTextureIndex() == -1)
                    Selection::SetCurrentMode(Selection::Mode::None);
                else
                    Selection::SetCurrentMode(Selection::Mode::Paint);
            }
            else
            {
				Selection::SetCurrentMode(Selection::Mode::Erase); 
            }
        }

        if (Selection::GetCurrentMode() == Selection::Mode::Erase)
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
            if (Selection::GetCurrentMode() == Selection::Mode::Fill)
            {
				if (m_TileAttributes->GetTextureIndex() == -1)
                    Selection::SetCurrentMode(Selection::Mode::None);
                else
					Selection::SetCurrentMode(Selection::Mode::Paint);
            }
            else
            {
                Selection::SetCurrentMode(Selection::Mode::Fill);
            }
        }

        if (Selection::GetCurrentMode() == Selection::Mode::Fill)
        {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();
            ImGui::GetWindowDrawList()->AddRect(min, max, Color::SELECTION_BORDER_COLOR, 5.0f, 0, 1.0f);
        }

        ImGui::PopID();

        ImGui::End();

        if (Selection::GetCurrentMode() == Selection::Mode::Erase)
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            ImVec2 mousePos = ImGui::GetMousePos();

            ImGui::GetForegroundDrawList()->AddImage(
                (void*)(intptr_t)m_EraserTexture->GetID(),
                ImVec2(mousePos.x - imageSize.x / 2, mousePos.y - imageSize.y / 2),
                ImVec2(mousePos.x + imageSize.x / 2, mousePos.y + imageSize.y / 2)
            );
        }

        if (Selection::GetCurrentMode() == Selection::Mode::Fill)
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