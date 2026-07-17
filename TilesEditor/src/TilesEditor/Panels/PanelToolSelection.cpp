#include "PanelToolSelection.h"
#include "../UIConstants.h"
#include "../UI/Widgets.h"
#include "imgui.h"

namespace Tiles::Editor
{
    PanelToolSelection::PanelToolSelection(EditorHost& host) : Panel(host)
    {
        LoadTextures();
    }

    void PanelToolSelection::Render()
    {
        ImGui::Begin("Tools", OpenFlag());

        RenderBlockToolButtons();
        RenderBlockBrushSize();
        RenderBlockShapeTools();
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

    void PanelToolSelection::RenderComponentToolButton(const char* id, ToolType toolType, const std::shared_ptr<Tiles::Texture>& texture, PaintingMode mode, const char* tooltip)
    {
        const ImVec2 size(UI::Tool::ButtonSize, UI::Tool::ButtonSize);

        if (!texture)
        {
            (void)UI::Button("?", UI::ButtonVariant::Default, size);
            return;
        }

        std::string buttonId = std::string("##") + id + "ToolButton";
        const ImTextureID textureId = static_cast<ImTextureID>(texture->GetID());

        if (UI::ImageToggleButton(buttonId.c_str(), textureId, IsToolSelected(mode), size))
            SetToolSelection(mode);

        ImGui::SetItemTooltip("%s", tooltip);
    }

    void PanelToolSelection::RenderBlockBrushSize()
    {
        // The brush/eraser footprint is an N x N block of cells.
        ImGui::Spacing();

        int brushSize = Ctx().GetBrushSize();
        ImGui::SetNextItemWidth(UI::Tool::ButtonSize * 3.0f + UI::Tool::ButtonSpacing * 2.0f);
        if (ImGui::SliderInt("Brush Size", &brushSize, 1, 10))
            Ctx().SetBrushSize(brushSize);
    }

    void PanelToolSelection::RenderBlockShapeTools()
    {
        ImGui::Spacing();

        const ImVec2 size(0.0f, UI::Tool::ButtonSize);

        if (UI::ToggleButton("Line", IsToolSelected(PaintingMode::Line), size))
            SetToolSelection(PaintingMode::Line);
        ImGui::SameLine();
        if (UI::ToggleButton("Rect", IsToolSelected(PaintingMode::Rectangle), size))
            SetToolSelection(PaintingMode::Rectangle);
        ImGui::SameLine();
        if (UI::ToggleButton("Circle", IsToolSelected(PaintingMode::Ellipse), size))
            SetToolSelection(PaintingMode::Ellipse);

        bool filled = Ctx().GetShapeFilled();
        if (ImGui::Checkbox("Filled", &filled))
            Ctx().SetShapeFilled(filled);
    }

    void PanelToolSelection::RenderBlockCustomCursor()
    {
        PaintingMode currentMode = Ctx().GetPaintingMode();

        if (currentMode == PaintingMode::None)
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
            return;
        }

        RenderComponentCursorForMode("BrushCursor", PaintingMode::Brush, m_BrushTexture);
        RenderComponentCursorForMode("EraserCursor", PaintingMode::Eraser, m_EraserTexture);
        RenderComponentCursorForMode("FillCursor", PaintingMode::Fill, m_FillTexture);
    }

    void PanelToolSelection::RenderComponentCursorForMode(const char* id, PaintingMode mode, const std::shared_ptr<Tiles::Texture>& texture)
    {
        if (Ctx().GetPaintingMode() != mode || !texture)
        {
            return;
        }

        // Hide the system cursor and draw the tool texture on the foreground draw
        // list so it appears above every window as the cursor.
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
        m_BrushTexture = Tiles::Texture::Create(AssetPath::Brush);
        m_EraserTexture = Tiles::Texture::Create(AssetPath::Eraser);
        m_FillTexture = Tiles::Texture::Create(AssetPath::Fill);
    }

    bool PanelToolSelection::IsToolSelected(PaintingMode mode) const
    {
        return Ctx().GetPaintingMode() == mode;
    }

    void PanelToolSelection::SetToolSelection(PaintingMode mode)
    {
        // Clicking the already-active tool toggles it off (back to None).
        if (Ctx().GetPaintingMode() == mode)
        {
            Ctx().SetPaintingMode(PaintingMode::None);
        }
        else
        {
            Ctx().SetPaintingMode(mode);
        }
    }
}