#include "PanelToolSelection.h"
#include "Core/Constants.h"
#include "imgui.h"

namespace Tiles
{
    PanelToolSelection::PanelToolSelection(Ref<Context> context) : Panel(context)
    {
        LoadTextures();
    }

    void PanelToolSelection::Render()
    {
        ImGui::Begin("Tools");

        if (!m_Context)
        {
            ImGui::TextColored(UI::Color::TextError, "No project loaded");
            ImGui::End();
            return;
        }

        RenderBlockToolButtons();
        ImGui::End();

        RenderBlockCustomCursor();
    }

    void PanelToolSelection::Update()
    {
    }

    void PanelToolSelection::RenderBlockToolButtons()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(UI::Tool::ButtonSpacing, 0.0f));

        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;
        if (ImGui::BeginTable("##ToolButtonsTable", 3, tableFlags))
        {
            // Setup columns
            ImGui::TableSetupColumn("Brush", ImGuiTableColumnFlags_WidthFixed, UI::Tool::ButtonSize);
            ImGui::TableSetupColumn("Eraser", ImGuiTableColumnFlags_WidthFixed, UI::Tool::ButtonSize);
            ImGui::TableSetupColumn("Fill", ImGuiTableColumnFlags_WidthFixed, UI::Tool::ButtonSize);
            ImGui::TableNextRow();

            // Brush tool
            ImGui::TableNextColumn();
            RenderComponentToolButton("BrushTool", ToolType::Brush, m_BrushTexture, PaintingMode::Brush, "Brush Tool");

            // Eraser tool
            ImGui::TableNextColumn();
            RenderComponentToolButton("EraserTool", ToolType::Eraser, m_EraserTexture, PaintingMode::Eraser, "Eraser Tool");

            // Fill tool
            ImGui::TableNextColumn();
            RenderComponentToolButton("FillTool", ToolType::Fill, m_FillTexture, PaintingMode::Fill, "Fill Tool");

            ImGui::EndTable();
        }

        ImGui::PopStyleVar();
    }

    void PanelToolSelection::RenderComponentToolButton(const char* id, ToolType toolType, const Lumina::Ref<Lumina::Texture>& texture, PaintingMode mode, const char* tooltip)
    {
        if (!texture)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_Button, UI::Color::BackgroundMedium);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Color::Hover);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Color::Active);
            ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::TextHint);
            ImGui::Button("?", ImVec2(UI::Tool::ButtonSize, UI::Tool::ButtonSize));
            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar();
            return;
        }

        std::string buttonId = std::string("##") + id + "ToolButton";

        bool isSelected = IsToolSelected(mode);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));

        if (isSelected)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, UI::Selection::BorderColor);
        }

        ImGui::PushStyleColor(ImGuiCol_Button, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Color::Hover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Color::Active);

        ImVec2 buttonSize(UI::Tool::ButtonSize, UI::Tool::ButtonSize);
        bool isPressed = ImGui::ImageButton(
            buttonId.c_str(),
            reinterpret_cast<void*>(static_cast<uintptr_t>(texture->GetID())),
            buttonSize
        );

        ImGui::PopStyleColor(3);

        if (isSelected)
        {
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }

        ImGui::PopStyleVar(2);

        if (isPressed)
        {
            SetToolSelection(mode);
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("%s", tooltip);
        }
    }

    void PanelToolSelection::RenderBlockCustomCursor()
    {
        if (!m_Context)
        {
            return;
        }

        PaintingMode currentMode = m_Context->GetPaintingMode();

        if (currentMode == PaintingMode::None)
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
            return;
        }

        RenderComponentCursorForMode("BrushCursor", PaintingMode::Brush, m_BrushTexture);
        RenderComponentCursorForMode("EraserCursor", PaintingMode::Eraser, m_EraserTexture);
        RenderComponentCursorForMode("FillCursor", PaintingMode::Fill, m_FillTexture);
    }

    void PanelToolSelection::RenderComponentCursorForMode(const char* id, PaintingMode mode, const Lumina::Ref<Lumina::Texture>& texture)
    {
        if (m_Context->GetPaintingMode() != mode || !texture)
        {
            return;
        }

        ImGui::SetMouseCursor(ImGuiMouseCursor_None);

        ImVec2 mousePos = ImGui::GetMousePos();

        float halfSize = UI::Tool::CursorSize * 0.5f;
        ImVec2 cursorMin = ImVec2(mousePos.x - halfSize, mousePos.y - halfSize);
        ImVec2 cursorMax = ImVec2(mousePos.x + halfSize, mousePos.y + halfSize);

        ImGui::GetForegroundDrawList()->AddImage(
            reinterpret_cast<void*>(static_cast<uintptr_t>(texture->GetID())),
            cursorMin,
            cursorMax,
            ImVec2(0, 0),
            ImVec2(1, 1),
            IM_COL32(255, 255, 255, 200)
        );
    }

    void PanelToolSelection::LoadTextures()
    {
        m_BrushTexture = Lumina::Texture::Create(AssetPath::Brush);
        m_EraserTexture = Lumina::Texture::Create(AssetPath::Eraser);
        m_FillTexture = Lumina::Texture::Create(AssetPath::Fill);
    }

    bool PanelToolSelection::IsToolSelected(PaintingMode mode) const
    {
        return m_Context && m_Context->GetPaintingMode() == mode;
    }

    void PanelToolSelection::SetToolSelection(PaintingMode mode)
    {
        if (!m_Context)
        {
            return;
        }

        if (m_Context->GetPaintingMode() == mode)
        {
            m_Context->SetPaintingMode(PaintingMode::None);
        }
        else
        {
            m_Context->SetPaintingMode(mode);
        }
    }
}