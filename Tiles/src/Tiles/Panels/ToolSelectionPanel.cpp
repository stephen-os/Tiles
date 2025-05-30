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

        RenderToolButton("EraserMode", m_EraserTexture, Modes::Mode::Erase);
        ImGui::SameLine();
        RenderToolButton("FillMode", m_FillTexture, Modes::Mode::Fill);

        ImGui::End();

        DrawCursorForMode(Modes::Mode::Erase, m_EraserTexture);
        DrawCursorForMode(Modes::Mode::Fill, m_FillTexture);
    }

    void ToolSelectionPanel::RenderToolButton(const char* id, const Shared<Lumina::Texture>& texture, Modes::Mode mode)
    {
        ImGui::PushID(id);

        if (ImGui::ImageButton((void*)(intptr_t)texture->GetID(), TOOL_IMAGE_SIZE))
        {
            if (Modes::GetCurrentMode() == mode)
            {
                if (true)
                    Modes::SetCurrentMode(Modes::Mode::None);
                else
                    Modes::SetCurrentMode(Modes::Mode::Paint);
            }
            else
            {
                Modes::SetCurrentMode(mode);
            }
        }

        if (Modes::GetCurrentMode() == mode)
        {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();
            ImGui::GetWindowDrawList()->AddRect(min, max, Color::SELECTION_BORDER_COLOR, 5.0f, 0, 1.0f);
        }

        ImGui::PopID();
    }

    void ToolSelectionPanel::DrawCursorForMode(Modes::Mode mode, const Shared<Lumina::Texture>& texture)
    {
        if (Modes::GetCurrentMode() != mode)
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
