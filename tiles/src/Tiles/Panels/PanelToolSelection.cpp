#include "PanelToolSelection.h"

#include "imgui.h"

namespace Tiles
{
    // Asset paths
    namespace AssetPaths
    {
        static constexpr const char* BRUSH_TEXTURE = "res/assets/brush.png";
        static constexpr const char* ERASER_TEXTURE = "res/assets/eraser.png";
        static constexpr const char* FILL_TEXTURE = "res/assets/bucket.png";
    }

    // UI layout constants
    namespace UILayout
    {
        static constexpr float DEFAULT_BUTTON_SIZE = 32.0f;
        static constexpr float SELECTION_BORDER_RADIUS = 5.0f;
        static constexpr float SELECTION_BORDER_THICKNESS = 1.0f;
    }

    // Tool identifiers
    namespace ToolIDs
    {
        static constexpr const char* BRUSH_MODE = "BrushMode";
        static constexpr const char* ERASER_MODE = "EraserMode";
        static constexpr const char* FILL_MODE = "FillMode";
    }

    namespace ColorScheme
    {
        static constexpr ImU32 SELECTED_BORDER = IM_COL32(255, 165, 0, 255);
        static constexpr ImU32 DEFAULT_BORDER = IM_COL32(51, 51, 51, 255);
    }

    PanelToolSelection::PanelToolSelection()
        : m_ButtonSize(UILayout::DEFAULT_BUTTON_SIZE, UILayout::DEFAULT_BUTTON_SIZE)
    {
        // Load tool textures
        m_BrushTexture = Texture::Create(AssetPaths::BRUSH_TEXTURE);
        m_EraserTexture = Texture::Create(AssetPaths::ERASER_TEXTURE);
        m_FillTexture = Texture::Create(AssetPaths::FILL_TEXTURE);
    }

    void PanelToolSelection::Render()
    {
        ImGui::Begin("Tools");

        if (!m_Context)
        {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No project loaded");
            ImGui::End();
            return;
        }

        RenderToolButtons();

        ImGui::End();

        // Render custom cursor overlay
        RenderCustomCursor();
    }

    void PanelToolSelection::Update()
    {
        // This panel does not require any specific update logic
	}

    void PanelToolSelection::RenderToolButtons()
    {
        // Render tool buttons in a horizontal layout
        RenderToolButton(ToolIDs::BRUSH_MODE, m_BrushTexture, PaintingMode::Brush);
        ImGui::SameLine();

        RenderToolButton(ToolIDs::ERASER_MODE, m_EraserTexture, PaintingMode::Eraser);
        ImGui::SameLine();

        RenderToolButton(ToolIDs::FILL_MODE, m_FillTexture, PaintingMode::Fill);
    }

    void PanelToolSelection::RenderToolButton(const char* id, const Ref<Lumina::Texture>& texture, PaintingMode mode)
    {
        if (!texture)
        {
            return; // Skip if texture failed to load
        }

        ImGui::PushID(id);

        // Create image button
        bool isPressed = ImGui::ImageButton(reinterpret_cast<void*>(static_cast<uintptr_t>(texture->GetID())), { m_ButtonSize.x, m_ButtonSize.y });

        // Handle button press
        if (isPressed)
        {
            // Toggle: if this mode is already selected, switch to None; otherwise switch to this mode
            if (m_Context->GetPaintingMode() == mode)
            {
                m_Context->SetPaintingMode(PaintingMode::None);
            }
            else
            {
                m_Context->SetPaintingMode(mode);
            }
        }

        // Draw selection border if this tool is active
        if (m_Context->GetPaintingMode() == mode)
        {
            ImVec2 buttonMin = ImGui::GetItemRectMin();
            ImVec2 buttonMax = ImGui::GetItemRectMax();

            ImGui::GetWindowDrawList()->AddRect(
                buttonMin,
                buttonMax,
				ColorScheme::SELECTED_BORDER,
                UILayout::SELECTION_BORDER_RADIUS,
                0,
                UILayout::SELECTION_BORDER_THICKNESS
            );
        }

        ImGui::PopID();
    }

    void PanelToolSelection::RenderCustomCursor()
    {
        if (!m_Context)
        {
            return;
        }

        PaintingMode currentMode = m_Context->GetPaintingMode();

        // Only render custom cursor for non-None modes
        if (currentMode == PaintingMode::None)
        {
            // Use default mouse cursor for None mode
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
            return;
        }

        // Draw custom cursor for active painting modes
        DrawCursorForMode(PaintingMode::Brush, m_BrushTexture);
        DrawCursorForMode(PaintingMode::Eraser, m_EraserTexture);
        DrawCursorForMode(PaintingMode::Fill, m_FillTexture);
    }

    void PanelToolSelection::DrawCursorForMode(PaintingMode mode, const Ref<Lumina::Texture>& texture)
    {
        // Only draw cursor for the currently active mode
        if (m_Context->GetPaintingMode() != mode || !texture)
        {
            return;
        }

        // Hide default mouse cursor and draw custom cursor
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);

        // Get current mouse position
        ImVec2 mousePos = ImGui::GetMousePos();

        // Calculate cursor image bounds (centered on mouse)
        ImVec2 halfSize = ImVec2(m_ButtonSize.x * 0.5f, m_ButtonSize.y * 0.5f);
        ImVec2 cursorMin = ImVec2(mousePos.x - halfSize.x, mousePos.y - halfSize.y);
        ImVec2 cursorMax = ImVec2(mousePos.x + halfSize.x, mousePos.y + halfSize.y);

        // Draw cursor image on foreground layer
        ImGui::GetForegroundDrawList()->AddImage(
            reinterpret_cast<void*>(static_cast<uintptr_t>(texture->GetID())),
            cursorMin,
            cursorMax
        );
    }
}