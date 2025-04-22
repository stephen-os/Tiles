#include "ToolSelectionPanel.h"

#include "../Core/Color.h"

namespace Tiles
{
    static constexpr const char* ERASER_TEXTURE_PATH = "res/assets/eraser.png";
    static constexpr const char* FILL_TEXTURE_PATH = "res/assets/bucket.png";
    static constexpr ImVec2 TOOL_IMAGE_SIZE = { 32, 32 };

    ToolSelectionPanel::ToolSelectionPanel()
    {
        m_EraserTexture = Lumina::Texture::Create(std::string(ERASER_TEXTURE_PATH));
        m_FillTexture = Lumina::Texture::Create(std::string(FILL_TEXTURE_PATH));
    }

    void ToolSelectionPanel::Render()
    {
        ImGui::Begin("Tools");

        RenderToolButton("EraserMode", m_EraserTexture, Selection::Mode::Erase);
        ImGui::SameLine();
        RenderToolButton("FillMode", m_FillTexture, Selection::Mode::Fill);

        ImGui::End();

        DrawCursorForMode(Selection::Mode::Erase, m_EraserTexture);
        DrawCursorForMode(Selection::Mode::Fill, m_FillTexture);
    }

    void ToolSelectionPanel::RenderToolButton(const char* id, const Shared<Lumina::Texture>& texture, Selection::Mode mode)
    {
        ImGui::PushID(id);

        if (ImGui::ImageButton((void*)(intptr_t)texture->GetID(), TOOL_IMAGE_SIZE))
        {
            if (Selection::GetCurrentMode() == mode)
            {
                if (true)
                    Selection::SetCurrentMode(Selection::Mode::None);
                else
                    Selection::SetCurrentMode(Selection::Mode::Paint);
            }
            else
            {
                Selection::SetCurrentMode(mode);
            }
        }

        if (Selection::GetCurrentMode() == mode)
        {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();
            ImGui::GetWindowDrawList()->AddRect(min, max, Color::SELECTION_BORDER_COLOR, 5.0f, 0, 1.0f);
        }

        ImGui::PopID();
    }

    void ToolSelectionPanel::DrawCursorForMode(Selection::Mode mode, const Shared<Lumina::Texture>& texture)
    {
        if (Selection::GetCurrentMode() != mode)
            return;

        ImGui::SetMouseCursor(ImGuiMouseCursor_None);
        ImVec2 mousePos = ImGui::GetMousePos();

        ImGui::GetForegroundDrawList()->AddImage(
            (void*)(intptr_t)texture->GetID(),
            ImVec2(mousePos.x - TOOL_IMAGE_SIZE.x / 2, mousePos.y - TOOL_IMAGE_SIZE.y / 2),
            ImVec2(mousePos.x + TOOL_IMAGE_SIZE.x / 2, mousePos.y + TOOL_IMAGE_SIZE.y / 2)
        );
    }
}
