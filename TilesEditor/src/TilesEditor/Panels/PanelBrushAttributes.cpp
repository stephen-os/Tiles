#include "PanelBrushAttributes.h"

#include "../UIConstants.h"
#include "../UI/Widgets.h"

#include <glm/gtc/type_ptr.hpp>

namespace Tiles::Editor
{
	// A full-cell-width themed button for the preset / reset tables.
	static bool PresetButton(const char* label, UI::ButtonVariant variant)
	{
		return UI::Button(label, variant, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
	}

	// Renders the panel window.
	void PanelBrushAttributes::Render()
	{
		ImGui::Begin("Brush Attributes", OpenFlag(), ImGuiWindowFlags_NoScrollWithMouse);

		RenderBlockBrushAttributes();

		ImGui::End();
	}

	void PanelBrushAttributes::Update()
	{
	}

	// Renders the four attribute sections under one id scope.
	void PanelBrushAttributes::RenderBlockBrushAttributes()
	{
		ImGui::PushID("BrushAttributes");

		RenderSectionRotation();
		RenderSectionSize();
		RenderSectionTint();
		RenderSectionReset();

		ImGui::PopID();
	}

	// Rotation: a Vec3 drag plus 0/90/180/270 presets.
	void PanelBrushAttributes::RenderSectionRotation()
	{
		auto& brush = Ctx().GetBrush();

		UI::SectionHeader("Rotation");

		glm::vec3 rotation = brush.GetRotation();
		if (UI::DragVec3("Rotation", glm::value_ptr(rotation), 1.0f, Brush::Rotation::Min, Brush::Rotation::Max, "%.1f°"))
			brush.SetRotation(rotation);

		if (ImGui::BeginTable("##RotationPresets", 4, ImGuiTableFlags_SizingStretchSame))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); if (PresetButton("0°", UI::ButtonVariant::Default))   brush.SetRotation(glm::vec3(0.0f, 0.0f, Brush::Rotation::Zero));
			ImGui::TableNextColumn(); if (PresetButton("90°", UI::ButtonVariant::Default))  brush.SetRotation(glm::vec3(0.0f, 0.0f, Brush::Rotation::Quarter));
			ImGui::TableNextColumn(); if (PresetButton("180°", UI::ButtonVariant::Default)) brush.SetRotation(glm::vec3(0.0f, 0.0f, Brush::Rotation::Half));
			ImGui::TableNextColumn(); if (PresetButton("270°", UI::ButtonVariant::Default)) brush.SetRotation(glm::vec3(0.0f, 0.0f, Brush::Rotation::ThreeQuarter));
			ImGui::EndTable();
		}

		ImGui::Spacing();
	}

	// Size: a Vec2 (W/H) drag plus 0.5x / 1x / 2x / 3x presets.
	void PanelBrushAttributes::RenderSectionSize()
	{
		auto& brush = Ctx().GetBrush();

		UI::SectionHeader("Size");

		glm::vec2 size = brush.GetSize();
		if (UI::DragVec2("Size", glm::value_ptr(size), 0.1f, Brush::Size::Min, Brush::Size::Max, "%.2f", "W", "H"))
			brush.SetSize(size);

		if (ImGui::BeginTable("##SizePresets", 4, ImGuiTableFlags_SizingStretchSame))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); if (PresetButton("0.5x", UI::ButtonVariant::Default)) brush.SetSize(glm::vec2(Brush::Size::Half));
			ImGui::TableNextColumn(); if (PresetButton("1x", UI::ButtonVariant::Default))   brush.SetSize(glm::vec2(Brush::Size::Normal));
			ImGui::TableNextColumn(); if (PresetButton("2x", UI::ButtonVariant::Default))   brush.SetSize(glm::vec2(Brush::Size::Double));
			ImGui::TableNextColumn(); if (PresetButton("3x", UI::ButtonVariant::Default))   brush.SetSize(glm::vec2(Brush::Size::Triple));
			ImGui::EndTable();
		}

		ImGui::Spacing();
	}

	// Tint: a Vec4 (RGBA) drag, a colour picker, and colour presets.
	void PanelBrushAttributes::RenderSectionTint()
	{
		auto& brush = Ctx().GetBrush();

		UI::SectionHeader("Tint");

		glm::vec4 tint = brush.GetTint();
		bool changed = UI::DragVec4("Tint", glm::value_ptr(tint), 0.01f, 0.0f, 1.0f, "%.2f", "R", "G", "B", "A");
		changed |= UI::ColorField("TintPicker", glm::value_ptr(tint));
		if (changed)
			brush.SetTint(tint);

		ImGui::Spacing();

		if (ImGui::BeginTable("##TintPresets", 4, ImGuiTableFlags_SizingStretchSame))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); if (PresetButton("White", UI::ButtonVariant::Default)) brush.SetTint(Brush::Tint::White);
			ImGui::TableNextColumn(); if (PresetButton("Red", UI::ButtonVariant::Default))   brush.SetTint(Brush::Tint::Red);
			ImGui::TableNextColumn(); if (PresetButton("Green", UI::ButtonVariant::Default)) brush.SetTint(Brush::Tint::Green);
			ImGui::TableNextColumn(); if (PresetButton("Blue", UI::ButtonVariant::Default))  brush.SetTint(Brush::Tint::Blue);
			ImGui::EndTable();
		}

		ImGui::Spacing();
	}

	// Reset: per-attribute resets plus a Reset All.
	void PanelBrushAttributes::RenderSectionReset()
	{
		auto& brush = Ctx().GetBrush();

		UI::SectionHeader("Reset");

		if (ImGui::BeginTable("##ResetButtons", 3, ImGuiTableFlags_SizingStretchSame))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); if (PresetButton("Reset Rotation", UI::ButtonVariant::Danger)) brush.SetRotation(glm::vec3(0.0f));
			ImGui::TableNextColumn(); if (PresetButton("Reset Size", UI::ButtonVariant::Danger))     brush.SetSize(glm::vec2(Brush::Size::Normal));
			ImGui::TableNextColumn(); if (PresetButton("Reset Tint", UI::ButtonVariant::Danger))     brush.SetTint(Brush::Tint::White);
			ImGui::EndTable();
		}

		ImGui::Spacing();

		if (UI::Button("Reset All", UI::ButtonVariant::Danger, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
		{
			brush.SetRotation(glm::vec3(0.0f));
			brush.SetSize(glm::vec2(Brush::Size::Normal));
			brush.SetTint(Brush::Tint::White);
		}
	}
}
