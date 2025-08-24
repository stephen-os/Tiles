#include "PanelBrushAttributes.h"

#include "Core/Constants.h"

namespace Tiles
{
    void PanelBrushAttributes::Render()
    {
        ImGui::Begin("Brush Attributes", nullptr, ImGuiWindowFlags_NoScrollWithMouse);

        if (!m_Context)
        {
            ImGui::TextColored(UI::Color::TextError, "No project loaded");
            ImGui::End();
            return;
        }

        RenderBlockBrushAttributes();

        ImGui::End();
    }

    void PanelBrushAttributes::Update()
    {
    }

    void PanelBrushAttributes::RenderBlockBrushAttributes()
    {
        // Set custom spacing for the entire attributes block
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(UI::Component::SpaceBetween, UI::Component::SpaceBetween));

        ImGui::PushID("BrushAttributes");

        RenderSectionRotation();
        RenderSectionSize();
        RenderSectionTint();
        RenderSectionReset();

        ImGui::PopID();
        ImGui::PopStyleVar();
    }

    void PanelBrushAttributes::RenderSectionRotation()
    {
        auto& brush = m_Context->GetBrush();
        auto rotation = brush.GetRotation();

        RenderComponentTitle("Rotation");

        RenderComponentVec3Controls("Rotation", rotation,
            Brush::Rotation::Min, Brush::Rotation::Max, "%.1f\u00B0");

        // Update brush if rotation changed
        if (rotation.x != brush.GetRotation().x || rotation.y != brush.GetRotation().y || rotation.z != brush.GetRotation().z)
        {
            brush.SetRotation(rotation);
        }

        // Render preset buttons in a table
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(UI::Component::SpaceBetween / 2.0f, 0.0f));
        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

        if (ImGui::BeginTable("##RotationPresets", 4, tableFlags))
        {
            ImGui::TableSetupColumn("Rot0", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Rot90", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Rot180", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Rot270", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            if (RenderComponentButton("Rot0", "0\u00B0", UI::Color::Button))
                brush.SetRotation(glm::vec3(0.0f, 0.0f, Brush::Rotation::Zero));

            ImGui::TableNextColumn();
            if (RenderComponentButton("Rot90", "90\u00B0", UI::Color::Button))
                brush.SetRotation(glm::vec3(0.0f, 0.0f, Brush::Rotation::Quarter));

            ImGui::TableNextColumn();
            if (RenderComponentButton("Rot180", "180\u00B0", UI::Color::Button))
                brush.SetRotation(glm::vec3(0.0f, 0.0f, Brush::Rotation::Half));

            ImGui::TableNextColumn();
            if (RenderComponentButton("Rot270", "270\u00B0", UI::Color::Button))
                brush.SetRotation(glm::vec3(0.0f, 0.0f, Brush::Rotation::ThreeQuarter));

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        ImGui::Spacing();
    }

    void PanelBrushAttributes::RenderSectionSize()
    {
        auto& brush = m_Context->GetBrush();
        auto size = brush.GetSize();

        RenderComponentTitle("Size");

        RenderComponentVec2Controls("Size", size,
            Brush::Size::Min, Brush::Size::Max, "%.2f", "W", "H");

        // Update brush if size changed
        if (size.x != brush.GetSize().x || size.y != brush.GetSize().y)
        {
            brush.SetSize(size);
        }

        // Render preset buttons in a table
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(UI::Component::SpaceBetween / 2.0f, 0.0f));
        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

        if (ImGui::BeginTable("##SizePresets", 4, tableFlags))
        {
            ImGui::TableSetupColumn("Size05", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Size1", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Size2", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Size3", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            if (RenderComponentButton("Size05", "0.5x", UI::Color::Button))
                brush.SetSize(glm::vec2(Brush::Size::Half));

            ImGui::TableNextColumn();
            if (RenderComponentButton("Size1", "1x", UI::Color::Button))
                brush.SetSize(glm::vec2(Brush::Size::Normal));

            ImGui::TableNextColumn();
            if (RenderComponentButton("Size2", "2x", UI::Color::Button))
                brush.SetSize(glm::vec2(Brush::Size::Double));

            ImGui::TableNextColumn();
            if (RenderComponentButton("Size3", "3x", UI::Color::Button))
                brush.SetSize(glm::vec2(Brush::Size::Triple));

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        ImGui::Spacing();
    }

    void PanelBrushAttributes::RenderSectionTint()
    {
        auto& brush = m_Context->GetBrush();
        auto& tint = brush.GetTint();

        RenderComponentTitle("Tint");

        RenderComponentVec4Controls("Tint", tint, 0.0f, 1.0f, "%.2f", "R", "G", "B", "A");

        // Update brush if tint changed
        if (tint.x != brush.GetTint().x || tint.y != brush.GetTint().y ||
            tint.z != brush.GetTint().z || tint.w != brush.GetTint().w)
        {
            brush.SetTint(tint);
        }

        RenderComponentColorPicker("TintPicker", tint);

        // Render preset buttons in a table
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(UI::Component::SpaceBetween / 2.0f, 0.0f));
        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

        if (ImGui::BeginTable("##TintPresets", 4, tableFlags))
        {
            ImGui::TableSetupColumn("TintWhite", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("TintRed", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("TintGreen", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("TintBlue", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            if (RenderComponentButton("TintWhite", "White", UI::Color::Button))
                brush.SetTint(Brush::Tint::White);

            ImGui::TableNextColumn();
            if (RenderComponentButton("TintRed", "Red", UI::Color::Button))
                brush.SetTint(Brush::Tint::Red);

            ImGui::TableNextColumn();
            if (RenderComponentButton("TintGreen", "Green", UI::Color::Button))
                brush.SetTint(Brush::Tint::Green);

            ImGui::TableNextColumn();
            if (RenderComponentButton("TintBlue", "Blue", UI::Color::Button))
                brush.SetTint(Brush::Tint::Blue);

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        ImGui::Spacing();
    }

    void PanelBrushAttributes::RenderSectionReset()
    {
        auto& brush = m_Context->GetBrush();

        RenderComponentTitle("Reset");

        // Render individual reset buttons in a table
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(UI::Component::SpaceBetween / 2.0f, 0.0f));
        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

        if (ImGui::BeginTable("##ResetButtons", 3, tableFlags))
        {
            ImGui::TableSetupColumn("ResetRot", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("ResetSize", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("ResetTint", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            if (RenderComponentButton("ResetRot", "Reset Rotation", UI::Color::Danger))
                brush.SetRotation(glm::vec3(0.0f));

            ImGui::TableNextColumn();
            if (RenderComponentButton("ResetSize", "Reset Size", UI::Color::Danger))
                brush.SetSize(glm::vec2(Brush::Size::Normal));

            ImGui::TableNextColumn();
            if (RenderComponentButton("ResetTint", "Reset Tint", UI::Color::Danger))
                brush.SetTint(Brush::Tint::White);

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        ImGui::Spacing();

        if (RenderComponentButton("ResetAll", "Reset All", UI::Color::Danger))
        {
            brush.SetRotation(glm::vec3(0.0f));
            brush.SetSize(glm::vec2(Brush::Size::Normal));
            brush.SetTint(Brush::Tint::White);
        }
    }

    void PanelBrushAttributes::RenderComponentTitle(const char* title)
    {
        ImGui::PushID(title);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Color::BackgroundMedium);
        ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);
        ImGui::Button(title, ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight));
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
        ImGui::PopID();

        ImGui::Spacing();
    }

    void PanelBrushAttributes::RenderComponentVec2Controls(const char* id, glm::vec2& vec, float minVal, float maxVal, const char* format, const char* xName, const char* yName)
    {
        // Define ID strings at the top
        std::string tableId = std::string("##") + id + "Table";
        std::string colXId = std::string(id) + "X";
        std::string colYId = std::string(id) + "Y";
        std::string labelXId = std::string(id) + "LabelX";
        std::string dragXId = std::string(id) + "DragX";
        std::string labelYId = std::string(id) + "LabelY";
        std::string dragYId = std::string(id) + "DragY";

        float framePaddingY = (UI::Component::ButtonHeight - ImGui::GetTextLineHeight()) * 0.5f;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(UI::Component::FramePadding, framePaddingY));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));

        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

        if (ImGui::BeginTable(tableId.c_str(), 2, tableFlags))
        {
            ImGui::TableSetupColumn(colXId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(colYId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            // X component
            ImGui::TableNextColumn();
            RenderComponentLabel(labelXId.c_str(), xName, UI::Color::Red);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentDragFloat(dragXId.c_str(), &vec.x, 0.1f, minVal, maxVal, format);

            // Y component
            ImGui::TableNextColumn();
            ImGui::Dummy(ImVec2(UI::Component::SpaceBetween, 0));
            ImGui::SameLine(0, 0);
            RenderComponentLabel(labelYId.c_str(), yName, UI::Color::Green);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentDragFloat(dragYId.c_str(), &vec.y, 0.1f, minVal, maxVal, format);

            ImGui::EndTable();
        }

        ImGui::PopStyleVar(2);
        ImGui::Spacing();
    }

    void PanelBrushAttributes::RenderComponentVec3Controls(const char* id, glm::vec3& vec, float minVal, float maxVal, const char* format, const char* xName, const char* yName, const char* zName)
    {
        // Define ID strings at the top
        std::string tableId = std::string("##") + id + "Table";
        std::string colXId = std::string(id) + "X";
        std::string colYId = std::string(id) + "Y";
        std::string colZId = std::string(id) + "Z";
        std::string labelXId = std::string(id) + "LabelX";
        std::string dragXId = std::string(id) + "DragX";
        std::string labelYId = std::string(id) + "LabelY";
        std::string dragYId = std::string(id) + "DragY";
        std::string labelZId = std::string(id) + "LabelZ";
        std::string dragZId = std::string(id) + "DragZ";

        float framePaddingY = (UI::Component::ButtonHeight - ImGui::GetTextLineHeight()) * 0.5f;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(UI::Component::FramePadding, framePaddingY));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));

        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

        if (ImGui::BeginTable(tableId.c_str(), 3, tableFlags))
        {
            ImGui::TableSetupColumn(colXId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(colYId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(colZId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            // X component
            ImGui::TableNextColumn();
            RenderComponentLabel(labelXId.c_str(), xName, UI::Color::Red);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentDragFloat(dragXId.c_str(), &vec.x, 1.0f, minVal, maxVal, format);

            // Y component
            ImGui::TableNextColumn();
            ImGui::Dummy(ImVec2(UI::Component::SpaceBetween, 0));
            ImGui::SameLine(0, 0);
            RenderComponentLabel(labelYId.c_str(), yName, UI::Color::Green);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentDragFloat(dragYId.c_str(), &vec.y, 1.0f, minVal, maxVal, format);

            // Z component
            ImGui::TableNextColumn();
            ImGui::Dummy(ImVec2(UI::Component::SpaceBetween, 0));
            ImGui::SameLine(0, 0);
            RenderComponentLabel(labelZId.c_str(), zName, UI::Color::Blue);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentDragFloat(dragZId.c_str(), &vec.z, 1.0f, minVal, maxVal, format);

            ImGui::EndTable();
        }

        ImGui::PopStyleVar(2);
        ImGui::Spacing();
    }

    void PanelBrushAttributes::RenderComponentVec4Controls(const char* id, glm::vec4& vec, float minVal, float maxVal, const char* format, const char* xName, const char* yName, const char* zName, const char* wName)
    {
        // Define ID strings at the top
        std::string tableId = std::string("##") + id + "Table";
        std::string colXId = std::string(id) + "X";
        std::string colYId = std::string(id) + "Y";
        std::string colZId = std::string(id) + "Z";
        std::string colWId = std::string(id) + "W";
        std::string labelXId = std::string(id) + "LabelX";
        std::string dragXId = std::string(id) + "DragX";
        std::string labelYId = std::string(id) + "LabelY";
        std::string dragYId = std::string(id) + "DragY";
        std::string labelZId = std::string(id) + "LabelZ";
        std::string dragZId = std::string(id) + "DragZ";
        std::string labelWId = std::string(id) + "LabelW";
        std::string dragWId = std::string(id) + "DragW";

        float framePaddingY = (UI::Component::ButtonHeight - ImGui::GetTextLineHeight()) * 0.5f;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(UI::Component::FramePadding, framePaddingY));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));

        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

        if (ImGui::BeginTable(tableId.c_str(), 4, tableFlags))
        {
            ImGui::TableSetupColumn(colXId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(colYId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(colZId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(colWId.c_str(), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            // X component
            ImGui::TableNextColumn();
            RenderComponentLabel(labelXId.c_str(), xName, UI::Color::Red);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentDragFloat(dragXId.c_str(), &vec.x, 0.01f, minVal, maxVal, format);

            // Y component
            ImGui::TableNextColumn();
            ImGui::Dummy(ImVec2(UI::Component::SpaceBetween, 0));
            ImGui::SameLine(0, 0);
            RenderComponentLabel(labelYId.c_str(), yName, UI::Color::Green);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentDragFloat(dragYId.c_str(), &vec.y, 0.01f, minVal, maxVal, format);

            // Z component
            ImGui::TableNextColumn();
            ImGui::Dummy(ImVec2(UI::Component::SpaceBetween, 0));
            ImGui::SameLine(0, 0);
            RenderComponentLabel(labelZId.c_str(), zName, UI::Color::Blue);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentDragFloat(dragZId.c_str(), &vec.z, 0.01f, minVal, maxVal, format);

            // W component
            ImGui::TableNextColumn();
            ImGui::Dummy(ImVec2(UI::Component::SpaceBetween, 0));
            ImGui::SameLine(0, 0);
            RenderComponentLabel(labelWId.c_str(), wName, UI::Color::Gray);
            ImGui::SameLine(0, UI::Component::SpaceBetween);
            RenderComponentDragFloat(dragWId.c_str(), &vec.w, 0.01f, minVal, maxVal, format);

            ImGui::EndTable();
        }

        ImGui::PopStyleVar(2);
        ImGui::Spacing();
    }

    void PanelBrushAttributes::RenderComponentLabel(const char* id, const char* label, const ImVec4& color)
    {
        ImGui::PushID(id);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
        ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);
        ImGui::Button(label, ImVec2(UI::Component::ButtonWidth, UI::Component::ButtonHeight));
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
        ImGui::PopID();
    }

    void PanelBrushAttributes::RenderComponentDragFloat(const char* id, float* value, float speed, float minVal, float maxVal, const char* format)
    {
        ImGui::PushID(id);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, UI::Color::Normal);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, UI::Color::Hover);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, UI::Color::Active);
        ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat("##drag", value, speed, minVal, maxVal, format);
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
        ImGui::PopID();
    }

    void PanelBrushAttributes::RenderComponentColorPicker(const char* id, glm::vec4& color)
    {
        ImGui::PushID(id);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, ImGui::GetStyle().ItemSpacing.y));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, UI::Color::BackgroundDark);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, UI::Color::BackgroundDark);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, UI::Color::BackgroundDark);
        ImGui::SetNextItemWidth(-1);

        ImGuiColorEditFlags flags =
            ImGuiColorEditFlags_AlphaBar |
            ImGuiColorEditFlags_AlphaPreview |
            ImGuiColorEditFlags_DisplayRGB |
            ImGuiColorEditFlags_NoLabel;
        ImGui::ColorEdit4("##ColorPicker", glm::value_ptr(color), flags, UI::Component::SpaceBetween);

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);
        ImGui::PopID();

        ImGui::Spacing();
    }

    bool PanelBrushAttributes::RenderComponentButton(const char* id, const char* label, const ImVec4& buttonColor)
    {
        ImGui::PushID(id);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Color::ButtonHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Color::ButtonActive);
        ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);

        bool clicked = ImGui::Button(label, ImVec2(ImGui::GetContentRegionAvail().x, UI::Component::ButtonHeight));

        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
        ImGui::PopID();

        return clicked;
    }
}