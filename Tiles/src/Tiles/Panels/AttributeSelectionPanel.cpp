#include "AttributeSelectionPanel.h"

#include "imgui.h"

namespace Tiles
{
	void AttributeSelectionPanel::Render()
	{
		ImGui::Begin("2. Attribute Selection", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		glm::vec4& tintColor = m_TileAttributes->GetTintColor();

		ImGui::Text("Tint Selection");
		ImGui::ColorPicker4("##TintColor", &tintColor[0], ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_PickerHueBar);
		
		ImGui::Spacing();

		if (ImGui::Button("Reset Tint"))
		{
			tintColor = glm::vec4(1.0f);
		}

		ImGui::Spacing();

		ImGui::Text("Rotation Selection");
        ImGui::Checkbox("Lock to 90°", &m_RotationLocked);

		float rotation = glm::degrees(m_TileAttributes->GetRotation());

        if (m_RotationLocked)
        {
            int stepOptions[] = { 0, 90, 180, 270 };
            const char* labels[] = { "0 degrees", "90 degrees", "180 degrees", "270 degrees" };

            int currentStep = ((int)rotation + 45) / 90 % 4;

            if (ImGui::Combo("Rotation", &currentStep, labels, IM_ARRAYSIZE(labels)))
            {
                m_TileAttributes->SetRotation(glm::radians(static_cast<float>(stepOptions[currentStep])));
            }
        }
        else
        {
            if (ImGui::SliderFloat("Rotation (°)", &rotation, 0.0f, 360.0f))
            {
                m_TileAttributes->SetRotation(rotation);
            }
        }

		ImGui::End();
	}
}
